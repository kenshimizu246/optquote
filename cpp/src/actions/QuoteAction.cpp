
#include <iostream>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

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
#include "../Utils.hpp"
#include "QuoteAction.hpp"

using namespace rapidjson;

namespace optquote {

#define STR(var) #var


QuoteAction::QuoteAction(AppCtx* appCtx, shared_ptr<Session> session) : appCtx(appCtx), session(session) {
}

QuoteAction::~QuoteAction() {
	BOOST_LOG_TRIVIAL(info) << "QuoteAction:delete:start";
	BOOST_LOG_TRIVIAL(info) << "QuoteAction:delete:end";
}

void QuoteAction::doAction(unique_ptr<Document> rq, string& ss){
	Document res;
	Document::AllocatorType& alloc = res.GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("quote_result");

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

	if((*rq)["body"]["conversation"].HasMember("quoteStart")){
		time_t quoteStart = Utils::convertToTimeT((*rq)["body"]["conversation"]["quoteStart"].GetString());
		if(quoteStart != conv->quoteStart) {
			throw invalid_argument("invalid quote start time!");
		}
	}

	if((*rq)["body"]["conversation"].HasMember("quoteExpire")){
		time_t quoteExpire = Utils::convertToTimeT((*rq)["body"]["conversation"]["quoteExpire"].GetString());
		if(quoteExpire != conv->quoteExpire) {
			throw invalid_argument("invalid quote expire time!");
		}
	}

	long now = time(nullptr);
	conv->status = Status::QUOTE;
	conv->quoteRemain = conv->quoteExpire - now;
	
	dao->quote(conv);
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
}

}
