
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

#include "ActionExecutor.hpp"
#include "actions/ErrorAction.hpp"

using namespace rapidjson;

namespace optquote {

PThreadDetachException::PThreadDetachException(int error) : error(error) {

}

ActionExecutor::ActionExecutor(unique_ptr<Action> action, shared_ptr<WebSocketWriter> writer, unique_ptr<Document> rq)
				: Executor(move(rq)), action(move(action)), writer(writer)
{
}

ActionExecutor::~ActionExecutor() {
	BOOST_LOG_TRIVIAL(info) << "ActionExecutor::destructor.";
}

void ActionExecutor::execute(){
	string ss;
	int error = 0;

	try{
		BOOST_LOG_TRIVIAL(info) << "ActionExecutor::execute : start..." << (*rq)["header"]["type"].GetString();

		if(action){
			action->doAction(move(rq), ss);
		}
		BOOST_LOG_TRIVIAL(info) << "ActionExecutor::execute : end...";
	} catch (invalid_argument ex){
		BOOST_LOG_TRIVIAL(error) << "catch exception : " << ex.what();
		ErrorAction errorAction(string(ex.what()));
		errorAction.doAction(move(rq), ss);
	} catch (...){
		BOOST_LOG_TRIVIAL(error) << "catch something!";
		ErrorAction errorAction("catch something!");
		errorAction.doAction(move(rq), ss);
	}
	int i = 0;
	if(writer){
		i = writer->write(ss);
	}
	BOOST_LOG_TRIVIAL(info) << "ActionExecutor::execute::lws_write : " << i;
}

}

