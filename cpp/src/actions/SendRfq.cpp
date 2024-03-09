
#include <iostream>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <memory>

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
#include "SendRfq.hpp"

using namespace rapidjson;

namespace optquote {

#define STR(var) #var

SendRfq::SendRfq(AppCtx* appCtx, shared_ptr<Session> session) : appCtx(appCtx), session(session) {
}

SendRfq::~SendRfq() {
	BOOST_LOG_TRIVIAL(info) << "SendRfq:delete:start";
	BOOST_LOG_TRIVIAL(info) << "SendRfq:delete:end";
}

void SendRfq::doAction(unique_ptr<Document> rq, string& ss){
	Document res;
	Document::AllocatorType& alloc = res.GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("send_rfq_result");

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);

	if(!(*rq)["body"].HasMember("conversation")){
		throw invalid_argument("conversation is mandatory!");
	}

	if(!(*rq)["body"]["conversation"].HasMember("status")){
		throw invalid_argument("status is mandatory!");
	}

	if(!(*rq)["body"]["conversation"].HasMember("tradeDate")){
		throw invalid_argument("tradeDate is mandatory!");
	}

	if(!(*rq)["body"]["conversation"].HasMember("settlementDate")){
		throw invalid_argument("settlement is mandatory!");
	}

	if(!(*rq)["body"]["conversation"].HasMember("side")){
		throw invalid_argument("side is mandatory!");
	}

	if(!(*rq)["body"]["conversation"].HasMember("symbol")){
		throw invalid_argument("symbol is mandatory!");
	}

	if(!(*rq)["body"]["conversation"].HasMember("spotPrice")){
		throw invalid_argument("spotPrice is mandatory!");
	}

	if(!(*rq)["body"]["conversation"]["spotPrice"].IsNumber()){
		throw invalid_argument("spotPrice must be number!");
	}

	if(!(*rq)["body"]["conversation"].HasMember("riskFreeRate")){
		throw invalid_argument("riskFreeRate is mandatory!");
	}

	if(!(*rq)["body"]["conversation"]["riskFreeRate"].IsNumber()){
		throw invalid_argument("riskFreeRate must be number!");
	}

	if(!(*rq)["body"]["conversation"].HasMember("dividendYield")){
		throw invalid_argument("dividendYield is mandatory!");
	}

	if(!(*rq)["body"]["conversation"]["dividendYield"].IsNumber()){
		throw invalid_argument("dividendYield must be number!");
	}

	if(!(*rq)["body"]["conversation"].HasMember("legs")){
		throw invalid_argument("legs is mandatory!");
	}

	if( (*rq)["body"]["conversation"]["legs"].GetType() != kArrayType){
		throw invalid_argument("legs must must be array!");
	}

	shared_ptr<Dao> dao = appCtx->getDaoFactory()->create();

	shared_ptr<Conversation> conv = shared_ptr<Conversation>(new Conversation());

	conv->status = Status::RFQ;
	if((*rq)["body"]["conversation"].HasMember("oc")){
		conv->oc = conv->toOC((*rq)["body"]["conversation"]["oc"].GetString());
	} else {
		conv->oc = OC::Open;
	}
	if((*rq)["body"]["conversation"].HasMember("link_id")){
		conv->link_id = (*rq)["body"]["conversation"]["link_id"].GetInt();
	}else{
		conv->link_id = 0;
	}
	conv->tradeDate = Utils::convertToTimeT((*rq)["body"]["conversation"]["tradeDate"].GetString());
	conv->settlementDate = Utils::convertToTimeT((*rq)["body"]["conversation"]["settlementDate"].GetString());
	conv->userId = session->userId;
	conv->side = (string((*rq)["body"]["conversation"]["side"].GetString()).compare("Sell") == 0 ? Sell : Buy);
	conv->symbol = (*rq)["body"]["conversation"]["symbol"].GetString();
	conv->spotPrice = (*rq)["body"]["conversation"]["spotPrice"].GetDouble();
	conv->riskFreeRate = (*rq)["body"]["conversation"]["riskFreeRate"].GetDouble();
	conv->dividendYield = (*rq)["body"]["conversation"]["dividendYield"].GetDouble();
	conv->strategy = string((*rq)["body"]["conversation"]["strategy"].GetString());

	for(auto& v : (*rq)["body"]["conversation"]["legs"].GetArray()){
		shared_ptr<Leg> l = shared_ptr<Leg>(new Leg(
								v["id"].GetInt(),
								(string(v["ls"].GetString()).compare("Long") == 0 ? Long : Short),
								(string(v["cp"].GetString()).compare("Call") == 0 ? Call : Put),
								v["quantity"].GetDouble(),
								v["strike"].GetDouble(),
								 Utils::convertToTimeT(v["maturity"].GetString())
								));
		if(v.HasMember("volatility")){
			l->volatility = v["volatility"].GetDouble();
		}
		conv->legs.push_back(l);
	}

	dao->sendRfq(conv);
	appCtx->getConversationManager()->add(conv);
	appCtx->getConversationManager()->sendConversationEvent(conv);

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

	BOOST_LOG_TRIVIAL(info) << "SendRfq::doAction::ss:" << ss;

	dao->commit();
	BOOST_LOG_TRIVIAL(info) << "End of SendRfq::doAction";
}

}
