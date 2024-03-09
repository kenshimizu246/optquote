
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

#include <libwebsockets.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include "NullAction.hpp"
#include "../AppCtx.hpp"

using namespace rapidjson;

namespace optquote {

NullAction::NullAction() {
}

NullAction::~NullAction() {
	syslog(LOG_INFO, "NullAction:delete:start");
	syslog(LOG_INFO, "NullAction:delete:end");
}

void NullAction::doAction(unique_ptr<Document> rq, string& ss){
	Document res;
	Document::AllocatorType& alloc = res.GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("null");

  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);

	hdr.AddMember("type", type, alloc);

	res.SetObject()
		.AddMember("header", hdr, alloc)
		.AddMember("body", bdy, alloc);

	res.Accept(writer);

	ss.append(buffer.GetString(), buffer.GetLength());
}

}


