#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <mutex>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include "Conversation.hpp"
#include "Leg.hpp"

using namespace std;

namespace optquote {

#define STR(var) #var

Conversation::Conversation(){
	quoteStart = 1;
	quoteExpire = 0;
}

Conversation::Conversation(Status status,
								OC oc,
                time_t tradeDate,
                time_t settlementDate,
                string userId,
                Side side,
                string symbol,
                double spotPrice,
                double riskFreeRate,
                double dividendYield,
                string strategy
                  ):
                tradeDate(tradeDate),
                settlementDate(settlementDate),
                userId(userId),
                side(side),
                symbol(symbol),
                spotPrice(spotPrice),
                riskFreeRate(riskFreeRate),
                dividendYield(dividendYield),
                strategy(strategy),
								quoteStart(1),
								quoteExpire(0)
{
}

Conversation::~Conversation(){
}

void Conversation::toJsonString(string& ss, shared_ptr<Session> session) {
	Document res;
	Document::AllocatorType& alloc = res.GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("conversation");

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);

	string st(toString(status));
	string soc(toString(oc));
	char buff[20];
	Value cnv(kObjectType);

	cnv.AddMember("id", id, alloc);
	cnv.AddMember("status", Value(st.c_str(), alloc), alloc);
	cnv.AddMember("oc", Value(soc.c_str(), alloc), alloc);
	cnv.AddMember("link_id", link_id, alloc);

	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&tradeDate));
	cnv.AddMember("tradeDate", Value(buff, alloc), alloc);
	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&settlementDate));
	cnv.AddMember("settlementDate", Value(buff, alloc), alloc);
	cnv.AddMember("userId", Value(userId.c_str(), alloc), alloc);
	cnv.AddMember("side", Value(side == Buy ? STR(Buy) : STR(Sell), alloc), alloc);
	cnv.AddMember("symbol", Value(symbol.c_str(), alloc), alloc);
	cnv.AddMember("spotPrice", spotPrice, alloc);
	cnv.AddMember("riskFreeRate", riskFreeRate, alloc);
	cnv.AddMember("dividendYield", dividendYield, alloc);
	cnv.AddMember("strategy", Value(strategy.c_str(),alloc), alloc);
	cnv.AddMember("traderId", Value(traderId.c_str(), alloc), alloc);
	if(quoteStart > 1){
		strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&quoteStart));
		cnv.AddMember("quoteStart", Value(buff, alloc), alloc);
		strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&quoteExpire));
		cnv.AddMember("quoteExpire", Value(buff, alloc), alloc);
		cnv.AddMember("quoteRemain", quoteRemain, alloc);
	}

	Value lgs(kArrayType);
	for(shared_ptr<Leg> leg : legs){
		Value ll(kObjectType);
		ll.AddMember("id", leg->id, alloc);
		ll.AddMember("ls", StringRef((leg->ls == Long ? STR(Long) : STR(Short))), alloc);
		ll.AddMember("cp", StringRef((leg->cp == Call ? STR(Call) : STR(Put))), alloc);
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

		lgs.PushBack(ll, alloc);
	}
	cnv.AddMember("legs", lgs, alloc);

	bdy.AddMember("conversation", cnv, alloc);

	hdr.AddMember("type", type, alloc);

	res.SetObject()
		.AddMember("header", hdr, alloc)
		.AddMember("body", bdy, alloc);

	res.Accept(writer);

	ss += buffer.GetString();
//	ss.append(buffer.GetString(), buffer.GetLength());
}

string Conversation::toString(Status status){
	switch(status){
		case Status::INIT:
			return STR(INIT);
		case Status::RFQ:
			return STR(RFQ);
		case Status::TAKE:
			return STR(TAKE);
		case Status::QUOTE:
			return STR(QUOTE);
		case Status::REJECT_RFQ:
			return STR(REJECT_RFQ);
		case Status::ORDER:
			return STR(ORDER);
		case Status::REJECT_QUOTE:
			return STR(REJECT_QUOTE);
		case Status::EXECUTE:
			return STR(EXECUTE);
		case Status::REJECT_ORDER:
			return STR(REJECT_ORDER);
		case Status::EXECUTE_ACK:
			return STR(EXECUTE_ACK);
		case Status::COUNTER:
			return STR(COUNTER);
		case Status::TIMEOUT:
			return STR(TIMEOUT);
	}
	return STR(INIT);
}

string Conversation::toString(OC oc){
	switch(oc){
		case OC::Open:
			return STR(Open);
		case OC::Close:
			return STR(Close);
	}
	return STR(Open);
}

OC Conversation::toOC(string s){
	if(s == "Open"){
		return  OC::Open;
	} else if(s == "Close"){
		return  OC::Close;
	}
	return OC::Open;
}

shared_ptr<Leg> Conversation::getLeg(int id){
	for(auto l : legs){
		if(l->id == id){
			return l;
		}
	}
	return nullptr;
}

}


