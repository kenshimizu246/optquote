#include <iostream>
#include <sstream>

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

#include "HeartbeatAction.hpp"
#include "../AppCtx.hpp"

using namespace rapidjson;
using namespace std;

namespace optquote {

HeartbeatAction::HeartbeatAction(AppCtx *appCtx) : appCtx(appCtx) {
}

HeartbeatAction::~HeartbeatAction(){
	BOOST_LOG_TRIVIAL(info) << "HeartbeatAction::~HeartbeatAction()";
}

void HeartbeatAction::doAction(unique_ptr<Document> rq, string& ss){
	Document res;
	Document::AllocatorType& alloc = res.GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);

  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);

	BOOST_LOG_TRIVIAL(info) << "HeartbeatAction::doAction()...start";

	hdr.AddMember("type", "heartbeat_response", alloc);

	bdy.AddMember("message", "Hello", alloc);

  res.SetObject()
    .AddMember("header", hdr, alloc)
    .AddMember("body", bdy, alloc);

  res.Accept(writer);

  ss.append(buffer.GetString(), buffer.GetLength());

	BOOST_LOG_TRIVIAL(info) << "HeartbeatAction::doAction()...end";
}

}


