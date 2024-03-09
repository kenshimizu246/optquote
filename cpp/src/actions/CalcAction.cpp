
#include <iostream>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <ctime>
#include <cmath>

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

#include <ql/quantlib.hpp>

#include "../AppCtx.hpp"
#include "../Dao.hpp"
#include "../Session.hpp"
#include "../Conversation.hpp"
#include "../Leg.hpp"
#include "../Config.hpp"
#include "CalcAction.hpp"

using namespace rapidjson;

namespace optquote {

#define STR(var) #var


CalcAction::CalcAction(AppCtx* appCtx, shared_ptr<Session> session) : appCtx(appCtx), session(session) {
}

CalcAction::~CalcAction() {
	BOOST_LOG_TRIVIAL(info) << "CalcAction:delete:start";
	BOOST_LOG_TRIVIAL(info) << "CalcAction:delete:end";
}

void CalcAction::doAction(unique_ptr<Document> rq, string& ss){
	Document res;
	Document::AllocatorType& alloc = res.GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("calc_quote_result");

	BOOST_LOG_TRIVIAL(info) << "CalcAction::start...";

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);

	if(!(*rq)["body"].HasMember("conversation")){
		throw invalid_argument("conversation is mandatory!");
	}

	if(!(*rq)["body"]["conversation"].HasMember("id")){
		throw invalid_argument("conversation.id is mandatory!");
	}

	int id = (*rq)["body"]["conversation"]["id"].GetInt();
	shared_ptr<Conversation> conv = appCtx->getConversationManager()->getConversation(id);

	if((*rq)["body"]["conversation"].HasMember("legs")){
		if( (*rq)["body"]["conversation"]["legs"].GetType() != kArrayType){
			throw invalid_argument("legs must must be array!");
		}
		for(auto& v : (*rq)["body"]["conversation"]["legs"].GetArray()){
			if(!v.HasMember("id")){
				throw invalid_argument("leg must have id!");
			}
			shared_ptr<Leg> l = conv->getLeg(v["id"].GetInt());
			if(l == nullptr){
				throw invalid_argument("leg is nullptr!");
			}
			if(v.HasMember("volatility")){
				l->volatility = v["volatility"].GetDouble();
			} else if(v.HasMember("premium")){
				l->premium = v["premium"].GetDouble();
			} else {
				throw invalid_argument("leg must have volatility or premium!");
			}
		}
	}

	
	// to prevent wrong decision, quoteStart is larger than qouteExpire
	time_t quoteStart = time(nullptr);
	time_t quoteExpire{0};
	long quoteRemain{0};

	quoteExpire = quoteStart + Config::getInstance().getQuoteLivePeriod();
	quoteRemain = quoteExpire - quoteStart;

	conv->quoteStart = quoteStart;
	conv->quoteExpire = quoteExpire;
	conv->quoteRemain = quoteRemain;

	BOOST_LOG_TRIVIAL(info) << "CalcAction::" << conv->id;

	QuantLib::Real spot           = conv->spotPrice;
	QuantLib::Rate riskFree       = conv->riskFreeRate;
	QuantLib::Rate dividendYield  = conv->dividendYield;
	for(shared_ptr<Leg> leg : conv->legs){
		QuantLib::Real strike         = leg->strike;
		QuantLib::Volatility sigma    = leg->volatility;

		double t2m = difftime(leg->maturity, conv->settlementDate) / (60*60*24);
		
		QuantLib::Real timeToMaturity = t2m / 365;

		QuantLib::Real vol = sigma * std::sqrt(timeToMaturity);

		QuantLib::DiscountFactor growth = std::exp(-dividendYield * timeToMaturity);

		QuantLib::DiscountFactor discount = std::exp(-riskFree * timeToMaturity);

		boost::shared_ptr<QuantLib::PlainVanillaPayoff> vanillaCallPayoff =
			boost::shared_ptr<QuantLib::PlainVanillaPayoff>(new QuantLib::PlainVanillaPayoff(QuantLib::Option::Type::Call, strike));

		QuantLib::BlackScholesCalculator bsCalculator(vanillaCallPayoff, spot, growth, vol, discount);

//		BOOST_LOG_TRIVIAL(info) << "BS : " << boost::format("Value of 110.0 call is %.4f") % bsCalculator.value();
//		BOOST_LOG_TRIVIAL(info) << "BS : " << boost::format("Delta of 110.0 call is %.4f") % bsCalculator.delta();
//		BOOST_LOG_TRIVIAL(info) << "BS : " << boost::format("Gamma of 110.0 call is %.4f") % bsCalculator.gamma();
//		BOOST_LOG_TRIVIAL(info) << "BS : " << boost::format("Vega of 110.0 call is %.4f") % (bsCalculator.vega(timeToMaturity)/100);
//		BOOST_LOG_TRIVIAL(info) << "BS : " << boost::format("Theta of 110.0 call is %.4f") % (bsCalculator.thetaPerDay(timeToMaturity));

		QuantLib::Real changeInSpot = 1.0;
		QuantLib::BlackScholesCalculator bsCalculatorSpotUpOneDollar(QuantLib::Option::Type::Call, strike, spot + changeInSpot, growth, vol, discount);

		BOOST_LOG_TRIVIAL(info) << "BS : value : " << bsCalculator.value();
		BOOST_LOG_TRIVIAL(info) << "BS : delta : " << bsCalculator.delta();
		if(!std::isnan(bsCalculator.value())){
			leg->premium = bsCalculator.value();
		} else {
			BOOST_LOG_TRIVIAL(info) << "CalcAction::BS Error value is NaN.";
		}
		if(!std::isnan(bsCalculator.delta())){
			leg->delta = bsCalculator.delta();
		} else {
			BOOST_LOG_TRIVIAL(info) << "CalcAction::BS Error delta is NaN.";
		}
		if(!std::isnan(bsCalculator.gamma())){
			leg->gamma = bsCalculator.gamma();
		} else {
			BOOST_LOG_TRIVIAL(info) << "CalcAction::BS Error gamma is NaN.";
		}
		if(!std::isnan(bsCalculator.vega(timeToMaturity))){
			leg->vega = (bsCalculator.vega(timeToMaturity)/100);
		} else {
			BOOST_LOG_TRIVIAL(info) << "CalcAction::BS Error vega is NaN.";
		}
		if(!std::isnan(bsCalculator.thetaPerDay(timeToMaturity))){
			leg->theta = bsCalculator.thetaPerDay(timeToMaturity);
		} else {
			BOOST_LOG_TRIVIAL(info) << "CalcAction::BS Error theta is NaN.";
		}
	}

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
	cnv.AddMember("traderId", StringRef(conv->traderId.c_str()), alloc);
	cnv.AddMember("side", StringRef(conv->side == Buy ? STR(Buy) : STR(Sell)), alloc);
	cnv.AddMember("symbol", StringRef(conv->symbol.c_str()), alloc);
	cnv.AddMember("spotPrice", conv->spotPrice, alloc);
	cnv.AddMember("riskFreeRate", conv->riskFreeRate, alloc);
	cnv.AddMember("dividendYield", conv->dividendYield, alloc);
	cnv.AddMember("strategy", StringRef(conv->strategy.c_str()), alloc);
	cnv.AddMember("oc", StringRef(oc.c_str()), alloc);
	cnv.AddMember("link_id", conv->link_id, alloc);
	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&conv->quoteStart));
	cnv.AddMember("quoteStart", StringRef(buff), alloc);
	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&conv->quoteExpire));
	cnv.AddMember("quoteExpire", StringRef(buff), alloc);
	cnv.AddMember("quoteRemain", conv->quoteRemain, alloc);

	Value legs(kArrayType);
	for(shared_ptr<Leg> leg : conv->legs){
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

BOOST_LOG_TRIVIAL(info) << "CalcAction-11:buffer:" << buffer.GetString();
BOOST_LOG_TRIVIAL(info) << "CalcAction-11:buffersize:" << buffer.GetLength();
	ss.append(buffer.GetString(), buffer.GetLength());

	//dao->commit();
//	delete dao;
BOOST_LOG_TRIVIAL(info) << "CalcAction-13";

}

}
