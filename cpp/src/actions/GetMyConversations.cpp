
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
#include "GetMyConversations.hpp"

using namespace rapidjson;

namespace optquote {

#define STR(var) #var

GetMyConversations::GetMyConversations(AppCtx* appCtx, shared_ptr<Session> session) : appCtx(appCtx), session(session) {
	BOOST_LOG_TRIVIAL(info) << "GetMyConversations::construct...";
}

GetMyConversations::~GetMyConversations() {
	BOOST_LOG_TRIVIAL(info) << "GetMyConversations::destruct...";
}

void GetMyConversations::doAction(unique_ptr<Document> rq, string& ss){
	vector<shared_ptr<Conversation>> vconvs;

	BOOST_LOG_TRIVIAL(info) << "GetMyConversations::doAction...";

	appCtx->getConversationManager()->getMyConversations(vconvs, session->userId);
	BOOST_LOG_TRIVIAL(info) << "GetMyConversations::vconvs:" << vconvs.size();

	Document res;
	Document::AllocatorType& alloc = res.GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("conversations");

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);

	Value conversations(kArrayType);

	for(shared_ptr<Conversation> p : vconvs){
		Value c(kObjectType);
		char buff[20];

		c.AddMember("id", Value(p->id), alloc);
		c.AddMember("status", Value(p->toString(p->status).c_str(), alloc), alloc);

		strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&p->tradeDate));
		c.AddMember("tradeDate", Value(buff, alloc), alloc);
		
		strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&p->settlementDate));
		c.AddMember("settlementDate", Value(buff, alloc), alloc);

		c.AddMember("userId", Value(p->userId.c_str(), alloc), alloc);
		c.AddMember("traderId", Value(p->traderId.c_str(), alloc), alloc);
		c.AddMember("side", Value(p->side == Buy ? STR(Buy) : STR(Sell), alloc), alloc);
		c.AddMember("symbol", Value(p->symbol.c_str(), alloc), alloc);
		c.AddMember("spotPrice", p->spotPrice, alloc);
		c.AddMember("riskFreeRate", p->riskFreeRate, alloc);
		c.AddMember("dividendYield", p->dividendYield, alloc);
		c.AddMember("strategy", Value(p->strategy.c_str(), alloc), alloc);

		Value legs(kArrayType);
		for(shared_ptr<Leg> l : p->legs){
			Value ll(kObjectType);
			ll.AddMember("id", l->id, alloc);
			ll.AddMember("ls", Value((l->ls == Long ? STR(Long) : STR(Short)), alloc), alloc);
			ll.AddMember("cp", Value((l->cp == Call ? STR(Call) : STR(Call)),alloc), alloc);
			ll.AddMember("quantity", l->quantity, alloc);
			ll.AddMember("strike", l->strike, alloc);
			ll.AddMember("volatility", l->volatility, alloc);

			ll.AddMember("premium", l->premium, alloc);
			ll.AddMember("delta", l->delta, alloc);
			ll.AddMember("gamma", l->gamma, alloc);
			ll.AddMember("vega", l->vega, alloc);
			ll.AddMember("theta", l->theta, alloc);

			strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&l->maturity));
			ll.AddMember("maturity", StringRef(buff), alloc);

			legs.PushBack(ll, alloc);
		}
		c.AddMember("legs",legs,alloc);

		conversations.PushBack(c, alloc);
	}
	bdy.AddMember("conversations", conversations, alloc);

	hdr.AddMember("type", type, alloc);

	res.SetObject()
		.AddMember("header", hdr, alloc)
		.AddMember("body", bdy, alloc);

	res.Accept(writer);

	ss.append(buffer.GetString(), buffer.GetLength());
}

}



