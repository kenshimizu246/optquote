
#include <iostream>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <syslog.h>
#include <pthread.h>
#include <thread>

#include <libwebsockets.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

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

#include "Config.hpp"
#include "AppCtx.hpp"
#include "LoginExecutor.hpp"
#include "AppWebSocketWriter.hpp"
#include "actions/ErrorAction.hpp"

using namespace std;
using namespace rapidjson;

namespace optquote {

LoginExecutor::LoginExecutor(shared_ptr<AppWebSocketWriter> writer, struct per_session_data_quote* pss, unique_ptr<Document> rq) : Executor(move(rq)), pss(pss), writer(writer) {
}

LoginExecutor::~LoginExecutor() {
}

void LoginExecutor::execute(){
	string ss;
	int error = 0;

	Document res;
	Document::AllocatorType& alloc = res.GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("login_result");

	StringBuffer buffer;
	Writer<StringBuffer> swriter(buffer);

	Value markets(kArrayType);

	try{
		BOOST_LOG_TRIVIAL(info) << "start execute...";

		hdr.AddMember("type", type, alloc);

		if(!(*rq)["body"].HasMember("userId")){
			throw invalid_argument("userId is not available!");
		}
		BOOST_LOG_TRIVIAL(debug) << "execute...userId:" << (*rq)["body"]["userId"].GetString();

		if(!(*rq)["body"].HasMember("passwd")){
			throw invalid_argument("passwd is not available!");
		}

		if(!(*rq)["body"].HasMember("role")){
			throw invalid_argument("role is not available!");
		}
		BOOST_LOG_TRIVIAL(debug) << "execute...role:" << (*rq)["body"]["role"].GetString();

		AppCtx *appCtx = Config::getInstance().getAppCtx();

		shared_ptr<Dao> dao = appCtx->getDaoFactory()->create();

		string userId((*rq)["body"]["userId"].GetString());
		string passwd((*rq)["body"]["passwd"].GetString());
		string role((*rq)["body"]["role"].GetString());

		shared_ptr<Session> session = appCtx->getSessionManager()->login(
				userId,
				passwd,
				role
				);
	
		if(session != nullptr){
			bdy.AddMember("sessionId", StringRef(session->id.c_str()), alloc);
			hdr.AddMember("sessionId", StringRef(session->id.c_str()), alloc);
			session->writer = writer;
			pss->session = session;
			pss->status = SESSION;
			pss->session_start_time = session->start_time;

			session->login_history_id = dao->addLoginHistory(
				userId,
				role,
				session->id
				);
	
			// start boot strap.
			thread tp([&dao,&session](){
				vector<shared_ptr<Price>> pp;
				dao->getPriceList(pp);
				BOOST_LOG_TRIVIAL(info) << "price size : " << pp.size();
				for(shared_ptr<Price> p : pp){
					string ss;
					p->toJsonString(ss);
					BOOST_LOG_TRIVIAL(info) << "price : " << ss;
					session->writer->write(ss);
				}
			});
	
			thread tc([&dao,&session,&userId,&appCtx](){
				vector<shared_ptr<Conversation>> cc;
				appCtx->getConversationManager()->getMyConversations(cc, userId);
				BOOST_LOG_TRIVIAL(info) << "conversation size : " << cc.size();
				for(shared_ptr<Conversation> c: cc){
					string ss;
					c->toJsonString(ss, session);
					BOOST_LOG_TRIVIAL(info) << "conversation : " << ss;
					session->writer->write(ss);
				}
			});

			tp.join();
			tc.join();
			// to detach, it need memory enhancement to manage conversation manager because of coredump
			// or try to join after sending result to client.
			//tp.detach();
			//tc.detach();
		} else {
			bdy.AddMember("sessionId", StringRef("ERROR"), alloc);
			hdr.AddMember("sessionId", StringRef("ERROR"), alloc);
			if(pss->session != nullptr){
				appCtx->getSessionManager()->logout(pss->session);
				pss->session = nullptr;
			}
		}

		res.SetObject()
			.AddMember("header", hdr, alloc)
			.AddMember("body", bdy, alloc);

		res.Accept(swriter);

		ss.append(buffer.GetString(), buffer.GetLength());
	} catch (invalid_argument ex){
		BOOST_LOG_TRIVIAL(info) << "catch exception : " << ex.what();
		ErrorAction errorAction(string(ex.what()));
		errorAction.doAction(move(rq), ss);
	} catch (...){
		ErrorAction errorAction("catch something!");
		errorAction.doAction(move(rq), ss);
		BOOST_LOG_TRIVIAL(info) << "catch something!";
	}

	BOOST_LOG_TRIVIAL(info) << "lws_write writes : " << ss;
	int i = writer->write(ss);
	BOOST_LOG_TRIVIAL(info) << "lws_write : " << i;
}

}

