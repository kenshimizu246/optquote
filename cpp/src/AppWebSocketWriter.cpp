#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <mutex>

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

#include "WebSocketWriter.hpp"
#include "AppWebSocketWriter.hpp"

using namespace std;

namespace optquote {

AppWebSocketWriter::AppWebSocketWriter(struct lws *wsi) : wsi(wsi){
}

AppWebSocketWriter::~AppWebSocketWriter(){
}

int AppWebSocketWriter::write(string& ss){
	BOOST_LOG_TRIVIAL(info) << "AppWebSocketWriter::lws_write:" << ss;
	lock_guard<mutex> lock(mtx);
	int i = lws_write(wsi, (unsigned char*)ss.c_str(), ss.length(), LWS_WRITE_TEXT);
	BOOST_LOG_TRIVIAL(info) << "AppWebSocketWriter::lws_write:" << i;
	return i;
}

}

