
#include <iostream>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <libwebsockets.h>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/named_scope.hpp>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include "AppCtx.hpp"
#include "Config.hpp"
#include "MariaCppConnPool.hpp"
#include "MariaDbDaoFactory.hpp"
#include "Executor.hpp"
#include "ActionExecutor.hpp"
#include "LoginExecutor.hpp"
#include "ActionFactory.hpp"
#include "actions/Action.hpp"
#include "actions/ErrorAction.hpp"
#include "Dao.hpp"
#include "session.h"
#include "AppWebSocketWriter.hpp"

#define DAEMON_NAME "simpledaemon"

using namespace std;
using namespace rapidjson;

namespace optquote {

class OptQuoteServer {
	public:
		OptQuoteServer();
		void init();
		void init_log();
		void daemonize(string rundir, string pidfile);
		void run();

	private:
		static void signal_handler(int sig);
		static void sighandler(int sig);
		static void daemonShutdown();
		static int  callback_quote_stock_option(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

		AppCtx appCtx;
		SessionManager sessionManager;
		MariaDbDaoFactory daoFactory;
		ConversationManager conversationManager;

		struct lws_protocols *pp;
		struct lws_context_creation_info info;
		int force_exitx;
		static int force_exit;
		static int pidFilehandle;
		static struct lws_context *context;
};

int OptQuoteServer::force_exit = 0;
int OptQuoteServer::pidFilehandle;
struct lws_context * OptQuoteServer::context;

OptQuoteServer::OptQuoteServer(){
	pp = new lws_protocols[2];
	pp[0].name = "quote";
	pp[0].callback = callback_quote_stock_option;
	pp[0].per_session_data_size = sizeof(struct per_session_data_quote);
	pp[0].rx_buffer_size = 8000;
	pp[1].name = NULL;
	pp[1].callback = NULL;
	pp[1].per_session_data_size = 0;
	pp[1].rx_buffer_size = 0;
	force_exitx = 0;
}

void OptQuoteServer::daemonShutdown(){
//	(pidFilehandle);
}

void OptQuoteServer::signal_handler(int sig){
	switch(sig){
		case SIGHUP:
		case SIGINT:
		case SIGTERM:
			daemonShutdown();
			exit(EXIT_SUCCESS);
			break;
		default:
			break;
	}
}

void OptQuoteServer::daemonize(string rundir, string pidfile) {
	int pid, sid, i;
	char str[10];
	struct sigaction newSigAction;
	sigset_t newSigSet;

	if (getppid() == 1) {
		return;
	}

	sigemptyset(&newSigSet);
	sigaddset(&newSigSet, SIGCHLD);
	sigaddset(&newSigSet, SIGTSTP);
	sigaddset(&newSigSet, SIGTTOU);
	sigaddset(&newSigSet, SIGTTIN);
	sigprocmask(SIG_BLOCK, &newSigSet, NULL);

	newSigAction.sa_handler = signal_handler;
	sigemptyset(&newSigAction.sa_mask);
	newSigAction.sa_flags = 0;

	sigaction(SIGHUP, &newSigAction, NULL);
	sigaction(SIGTERM, &newSigAction, NULL);
	sigaction(SIGINT, &newSigAction, NULL);

	// fork returns child process id if process is parent or 0 if process is child process
	// this kills the parent process to be independent process and under init/kernel process
	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	umask(027);

	sid = setsid();

	if (sid < 0) {
		exit(EXIT_FAILURE);
	}

	for (i = getdtablesize(); i >= 0; --i) {
		close(i);
	}

	i = open("/dev/null", O_RDWR);

	dup(i);
	dup(i);

	chdir(rundir.c_str());

	pidFilehandle = open(pidfile.c_str(), O_RDWR|O_CREAT, 0600);

	if (pidFilehandle == -1 ) {
		BOOST_LOG_TRIVIAL(info) << "daemonize.pidFilehandle" << pidfile;
		exit(EXIT_FAILURE);
	}

	if (lockf(pidFilehandle,F_TLOCK,0) == -1) {
		exit(EXIT_FAILURE);
	}

	sprintf(str,"%d\n",getpid());

	write(pidFilehandle, str, strlen(str));

	close(pidFilehandle);
}

void OptQuoteServer::sighandler(int sig)
{
	force_exit = 1;
	lws_cancel_service(context);
}

void OptQuoteServer::init(){
//	BOOST_LOG_TRIVIAL(info) << "end of server initialization... init";
}

void OptQuoteServer::init_log(){
//	BOOST_LOG_TRIVIAL(info) << "start of log initialization... init_log";

	boost::log::add_common_attributes();
//	boost::log::core::get()->add_global_attribute("Scope",
//		boost::log::attributes::named_scope());
	boost::log::core::get()->set_filter(
		boost::log::trivial::severity >= boost::log::trivial::trace
    );

	/* log formatter:
	 * [TimeStamp] [ThreadId] [Severity Level] [Scope] Log message
	 */
	auto fmtTimeStamp = boost::log::expressions::
		format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
	auto fmtThreadId = boost::log::expressions::
		attr<boost::log::attributes::current_thread_id::value_type>("ThreadID");
	auto fmtSeverity = boost::log::expressions::
		attr<boost::log::trivial::severity_level>("Severity");
	auto fmtScope = boost::log::expressions::format_named_scope("Scope",
		boost::log::keywords::format = "%n(%f:%l)",
		boost::log::keywords::iteration = boost::log::expressions::reverse,
		boost::log::keywords::depth = 2);
	boost::log::formatter logFmt =
		boost::log::expressions::format("[%1%] (%2%) [%3%] [%4%] %5%")
		% fmtTimeStamp % fmtThreadId % fmtSeverity % fmtScope
		% boost::log::expressions::smessage;

	/* console sink */
//	auto consoleSink = boost::log::add_console_log(std::clog);
//	consoleSink->set_formatter(logFmt);

		//boost::log::keywords::target = Config::getInstance().getLogDir().c_str(),
		//boost::log::keywords::target = "/var/log/optquote",
		//boost::log::keywords::file_name = "/var/log/optquote/test_%Y-%m-%d_%H-%M-%S.%N.log",
	string logdir{Config::getInstance().getLogDir() + "/test_%Y-%m-%d_%H-%M-%S.%N.log"};
	/* fs sink */
	auto fsSink = boost::log::add_file_log(
		boost::log::keywords::auto_flush = true,
		boost::log::keywords::file_name = logdir,
		boost::log::keywords::rotation_size = 10 * 1024 * 1024,
		boost::log::keywords::min_free_space = 30 * 1024 * 1024,
		boost::log::keywords::open_mode = std::ios_base::app);
	fsSink->set_formatter(logFmt);
//	fsSink->locked_backend()->auto_flush(true);

//	BOOST_LOG_TRIVIAL(info) << "end of log initialization... init_log";
}

void OptQuoteServer::run(){
	int opts = 0;
	int n = 0;
	unsigned int ms, oldms = 0;   

	/* Deamonize */
	if(Config::getInstance().isDaemon()){
		cout << "daemonize... " << Config::getInstance().getAppDir() << " : " << Config::getInstance().getPidFile() << endl;
		daemonize(Config::getInstance().getAppDir().c_str(), Config::getInstance().getPidFile().c_str());
		cout << "daemonized!" << endl;
	} else {
		cout << "not daemonized!" << endl;
	}

	init_log();

	conversationManager.addListener(&sessionManager);
	conversationManager.addSessionManager(&sessionManager);

	shared_ptr<Dao> dao = daoFactory.create();
	
	dao->load(conversationManager);
	dao->load(sessionManager);

	appCtx.setSessionManager(&sessionManager);
	appCtx.setConversationManager(&conversationManager);
	appCtx.setDaoFactory(&daoFactory);

	Config::getInstance().setAppCtx(&appCtx);

	memset(&info, 0, sizeof info);
	info.port = Config::getInstance().getPort();
	info.protocols = pp;
	info.ssl_cert_filepath = NULL;
	info.ssl_private_key_filepath = NULL;
	//info.extensions = lws_get_internal_extensions();
	info.extensions = NULL;

	info.gid = -1;
	info.uid = -1;
	info.max_http_header_pool = 1;
	info.options = opts;
    
	signal(SIGINT, sighandler);
 
	context = lws_create_context(&info);
	if (context == NULL) {
		lwsl_err("libwebsocket init failed\n");
		return; // -1;
	}
 
	// start conversation monitor...
	bool stopQt{false};
	ConversationManager& cm{conversationManager};

	thread qt([&cm,&stopQt]{
		struct timespec rq;

		rq.tv_sec = 1;
		rq.tv_nsec = 0; // 1000000 <- 1ms

		while(!stopQt){
			nanosleep(&rq, NULL);
			cm.checkTimeout();
		}	
	});
 
	n = 0;
	while (n >= 0 && !force_exit) {
		struct timeval tv;

		gettimeofday(&tv, NULL);

		ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
		if ((ms - oldms) > 50) {
			lws_callback_on_writable_all_protocol(context,
				&pp[0]);
			oldms = ms;
		}

		n = lws_service(context, 50);
	}//while n>=0
	BOOST_LOG_TRIVIAL(info) << "run while out";

	lws_context_destroy(context);
	BOOST_LOG_TRIVIAL(info) << "lws_context_destroy(context) end...";
}

int OptQuoteServer::callback_quote_stock_option(struct lws *wsi, enum lws_callback_reasons reason,
		void *user, void *in, size_t len)
{
	struct per_session_data_quote *pss = (struct per_session_data_quote *)user;
	int n;

	const string getqt("quote_request");
	const string eqopt("calc_equity_option");
	const string bondcalc("calc_bond");
	const string type_action("action");
	const string login("login");
	const string heartbeat("heartbeat");

	string symbol;

	unique_ptr<Document> rq;

	switch (reason) {
		case LWS_CALLBACK_ESTABLISHED:
			BOOST_LOG_TRIVIAL(info) << "LWS_CALLBACK_ESTABLISHED start...";
			pss->status = INIT;
			pss->buff = new string("");
			pss->session = nullptr;
			time(&(pss->start_time));
			BOOST_LOG_TRIVIAL(info) << "LWS_CALLBACK_ESTABLISHED end..." << pss->start_time;
			break;

		case LWS_CALLBACK_SERVER_WRITEABLE:
			if (pss->buff->length() < 1) {
				break;
			}

			BOOST_LOG_TRIVIAL(info) << "request processing : " << pss->buff->c_str();

			rq.reset(new Document());

			if ((*rq).Parse<0>(pss->buff->c_str()).HasParseError()){
				BOOST_LOG_TRIVIAL(info) << "HasParseError...";
			} else if ((*rq).HasMember("header")) {
				if ((*rq)["header"].HasMember("type")){
					BOOST_LOG_TRIVIAL(info) << "header.type : " << (*rq)["header"]["type"].GetString();
					if((*rq)["header"]["type"].GetString() == type_action) {
						ActionFactory af;
						unique_ptr<Executor> executor;
						AppCtx* appCtx = Config::getInstance().getAppCtx();

						if ((*rq)["header"].HasMember("action_name")){
							if ((*rq)["header"]["action_name"].GetString() == login) {
								executor.reset(new LoginExecutor(shared_ptr<AppWebSocketWriter>(new AppWebSocketWriter(wsi)), pss, move(rq)));
							} else {
								if ((*rq)["header"].HasMember("sessionId")) {
									string sessionId((*rq)["header"]["sessionId"].GetString());
									shared_ptr<Session> session = appCtx->getSessionManager()->getSessionBySessionID(sessionId);
									if(session != nullptr){
										// IMPORTANT: Enhancement is mandatory.
										shared_ptr<Dao> dao = appCtx->getDaoFactory()->create();
										dao->addAccessHistory(session);
										if(!session->writer){
											BOOST_LOG_TRIVIAL(info) << "assing writer to " << sessionId;
											session->writer = shared_ptr<AppWebSocketWriter>(new AppWebSocketWriter(wsi));
										}
										pss->session = session;
										string actionName((*rq)["header"]["action_name"].GetString());
										unique_ptr<Action> action{af.getActionHandler(actionName, sessionId, appCtx)};
										executor.reset(new ActionExecutor(move(action), session->writer, move(rq)));
									}else{
										BOOST_LOG_TRIVIAL(error) << "Session is not available! [" << sessionId << "]";
										executor.reset(new ActionExecutor(move(unique_ptr<Action>(new ErrorAction("your session is not available!"))), shared_ptr<AppWebSocketWriter>(new AppWebSocketWriter(wsi)), move(rq)));
									}
								} else {
									BOOST_LOG_TRIVIAL(error) << "Session ID is not available in the header!";
									executor.reset(new ActionExecutor(move(unique_ptr<Action>(new ErrorAction("your session Id is not available in your message header!"))), shared_ptr<AppWebSocketWriter>(new AppWebSocketWriter(wsi)), move(rq)));
								}
							}
							if(executor){
								BOOST_LOG_TRIVIAL(info) << "thread...executor";
								thread tth([px = move(executor)]{
									BOOST_LOG_TRIVIAL(info) << "before execute executor...";
									px->execute();
									BOOST_LOG_TRIVIAL(info) << "after execute executor...";
								});
								tth.detach();
							}
							BOOST_LOG_TRIVIAL(info) << "thread_handers.push_back...";
						} // end of if ((*rq)["header"].HasMember("action_name")){
					}
				}
			}
			BOOST_LOG_TRIVIAL(info) << "before delete string...";
			pss->buff->clear();
			break;

		case LWS_CALLBACK_RECEIVE:
			BOOST_LOG_TRIVIAL(info) << "LWS_CALLBACK_RECEIVE start...";
			pss->buff->append((const char*)in, len);
			lws_callback_on_writable_all_protocol(lws_get_context(wsi), lws_get_protocol(wsi));
			BOOST_LOG_TRIVIAL(info) << "LWS_CALLBACK_RECEIVE end... : " << pss->buff;
			break;

		case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
			break;

		case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE:
			BOOST_LOG_TRIVIAL(info) << "closing connection...:" << (pss == nullptr ? "nullptr" : "not nullptr");
			if(pss != nullptr){
				BOOST_LOG_TRIVIAL(info) << "closing connection... stat:" << pss->status
																<< " session:" << (pss->session == nullptr ? "nullptr" : "not nullptr")
																<< " pss len:" << len
																<< " start tm:" << pss->start_time;
				delete pss->buff;

				if(pss->status == SESSION &&  pss->session != nullptr){
					Config::getInstance().getAppCtx()->getSessionManager()->logout(pss->session);
				}
			}

			lwsl_notice("LWS_CALLBACK_WS_PEER_INITIATED_CLOSE: len %d\n", len);
			for (n = 0; n < (int)len; n++)
				lwsl_notice(" %d: 0x%02X\n", n, ((unsigned char *)in)[n]);
			BOOST_LOG_TRIVIAL(info) << "closing connection done...";
			break;

		default:
			break;
	}
	return 0;
}//callback_quote_stock_option

}


int main(int argc, char **argv)
{
	cout << "Start main..." << endl;

  if ( argc != 2) {
    cerr << "argument count mismatch error." << endl;
    exit(EXIT_FAILURE);
  }
	optquote::Config::getInstance().load(argv[1]);

	cout << "AppDir:" << optquote::Config::getInstance().getAppDir() << endl;
	cout << "LogDir:" << optquote::Config::getInstance().getLogDir() << endl;
	cout << "PidFile:" << optquote::Config::getInstance().getPidFile() << endl;

	optquote::OptQuoteServer server;

	server.init();

	cout << "after init..." << endl;

	server.run();
 
	BOOST_LOG_TRIVIAL(info) << "server.run() end...";

	return 0;
} //main
