
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
#include "ExecuteAction.hpp"

using namespace rapidjson;

namespace optquote {

#define STR(var) #var


ExecuteAction::ExecuteAction(AppCtx* appCtx, shared_ptr<Session> session) : appCtx(appCtx), session(session) {
}

ExecuteAction::~ExecuteAction() {
	BOOST_LOG_TRIVIAL(info) << "ExecuteAction:delete:start";
	BOOST_LOG_TRIVIAL(info) << "ExecuteAction:delete:end";
}

void ExecuteAction::doAction(unique_ptr<Document> rq, string& ss){
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
	
	conv->status = Status::EXECUTE;

	dao->execute(conv);
	appCtx->getConversationManager()->sendConversationEvent(conv);

	BOOST_LOG_TRIVIAL(info) << "Quote:" << conv->id;

	string status(conv->toString(conv->status));
	string oc(conv->toString(conv->oc));
	char buff[20];
	Value cnv(kObjectType);
	cnv.AddMember("id", conv->id, alloc);
	cnv.AddMember("status", StringRef(status.c_str()), alloc);
	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&conv->tradeDate));
	cnv.AddMember("tradeDate", StringRef(buff), alloc);
	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&conv->settlementDate));
	cnv.AddMember("settlementDate", StringRef(buff), alloc);
	cnv.AddMember("userId", StringRef(conv->userId.c_str()), alloc);
	cnv.AddMember("side", StringRef(conv->side == Buy ? STR(Buy) : STR(Sell)), alloc);
	cnv.AddMember("symbol", StringRef(conv->symbol.c_str()), alloc);
	cnv.AddMember("spotPrice", conv->spotPrice, alloc);
	cnv.AddMember("riskFreeRate", conv->riskFreeRate, alloc);
	cnv.AddMember("dividendYield", conv->dividendYield, alloc);
	cnv.AddMember("strategy", StringRef(conv->strategy.c_str()), alloc);
	cnv.AddMember("oc", StringRef(oc.c_str()), alloc);
	cnv.AddMember("link_id", conv->link_id, alloc);

BOOST_LOG_TRIVIAL(info) << "rfq-5";
	Value legs(kArrayType);
	for(shared_ptr<Leg> leg : conv->legs){
		Value ll(kObjectType);
		ll.AddMember("id", leg->id, alloc);
		ll.AddMember("ls", StringRef((leg->ls == Long ? STR(Long) : STR(Short))), alloc);
		ll.AddMember("cp", StringRef((leg->cp == Call ? STR(Call) : STR(Call))), alloc);
		ll.AddMember("quantity", leg->quantity, alloc);
		ll.AddMember("strike", leg->strike, alloc);
		ll.AddMember("volatility", leg->volatility, alloc);
		
		ll.AddMember("premium", leg->premium, alloc);
		ll.AddMember("delta", leg->delta, alloc);
		ll.AddMember("gamma", leg->gamma, alloc);
		ll.AddMember("vega", leg->vega, alloc);
		ll.AddMember("theta", leg->theta, alloc);

		strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&leg->maturity));
		ll.AddMember("maturity", StringRef(buff), alloc);

		legs.PushBack(ll, alloc);
	}
	cnv.AddMember("legs", legs, alloc);

	bdy.AddMember("conversation", cnv, alloc);
	bdy.AddMember("status", StringRef("OK"), alloc);

	hdr.AddMember("type", type, alloc);
	hdr.AddMember("sessionId", StringRef(session->id.c_str()), alloc);

	res.SetObject()
		.AddMember("header", hdr, alloc)
		.AddMember("body", bdy, alloc);

	res.Accept(writer);

BOOST_LOG_TRIVIAL(info) << "rfq-10";

BOOST_LOG_TRIVIAL(info) << "rfq-11";
	ss.append(buffer.GetString(), buffer.GetLength());

	dao->commit();
BOOST_LOG_TRIVIAL(info) << "rfq-13";
}

}

