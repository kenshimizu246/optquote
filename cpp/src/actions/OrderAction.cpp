
#include <iostream>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <stdexcept>

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

#include "../AppCtx.hpp"
#include "../Dao.hpp"
#include "../Session.hpp"
#include "../Conversation.hpp"
#include "../Leg.hpp"
#include "OrderAction.hpp"

using namespace rapidjson;

namespace optquote {

#define STR(var) #var


OrderAction::OrderAction(AppCtx* appCtx, shared_ptr<Session> session) : appCtx(appCtx), session(session) {
}

OrderAction::~OrderAction() {
	BOOST_LOG_TRIVIAL(info) << "OrderAction:delete:start";
	BOOST_LOG_TRIVIAL(info) << "OrderAction:delete:end";
}

void OrderAction::doAction(unique_ptr<Document> rq, string& ss){
	Document res;
	Document::AllocatorType& alloc = res.GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("order_result");

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);

	if(!(*rq)["body"].HasMember("conversation")){
		throw invalid_argument("conversation is mandatory!");
	}

	if(!(*rq)["body"]["conversation"].HasMember("id")){
		throw invalid_argument("conversation.id is mandatory!");
	}

	shared_ptr<Dao> dao = appCtx->getDaoFactory()->create();

	int id = (*rq)["body"]["conversation"]["id"].GetInt();
	shared_ptr<Conversation> conv = appCtx->getConversationManager()->getConversation(id);

	if(conv->status != Status::QUOTE){
		// BUG: should use proper exception class
		throw invalid_argument("current conversation.status must be QUOTE!");
	}

	time_t currentTime;
	time(&currentTime);

	if(currentTime <= conv->quoteExpire){	
		conv->status = Status::ORDER;
		dao->order(conv);
	} else {
		conv->status = Status::TIMEOUT;
		dao->timeoutQuote(conv);
	}

	appCtx->getConversationManager()->sendConversationEvent(conv);

	BOOST_LOG_TRIVIAL(info) << "Quote:" << conv->id;

	string status(conv->toString(conv->status));
	bdy.AddMember("result", StringRef("OK"), alloc);
	bdy.AddMember("id", conv->id, alloc);
	bdy.AddMember("status", StringRef(status.c_str()), alloc);

	hdr.AddMember("type", type, alloc);
	hdr.AddMember("sessionId", StringRef(session->id.c_str()), alloc);

	res.SetObject()
		.AddMember("header", hdr, alloc)
		.AddMember("body", bdy, alloc);

	res.Accept(writer);

	ss.append(buffer.GetString(), buffer.GetLength());

	dao->commit();
BOOST_LOG_TRIVIAL(info) << "rfq-13";
}

}
