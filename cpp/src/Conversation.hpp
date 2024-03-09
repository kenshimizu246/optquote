#include <string>
#include <sstream>
#include <vector>
#include <mutex>

#include <rapidjson/document.h>

#include "Session.hpp"
#include "Leg.hpp"

using namespace std;
using namespace rapidjson;

#ifndef _conversation_h
#define _conversation_h

namespace optquote {

enum Side{
	Buy=0,
	Sell=1
};

enum class Status{
INIT,
RFQ,
TAKE,
QUOTE,
REJECT_RFQ,
ORDER,
REJECT_QUOTE,
EXECUTE,
REJECT_ORDER,
EXECUTE_ACK,
COUNTER,
TIMEOUT,
};

enum class OC{
	Open=0,
	Close=1,
};

class Conversation {
	public:
		Conversation();
		Conversation(Status status,
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
									);
		~Conversation();

		void toJsonString(string& ss, shared_ptr<Session> session);
		shared_ptr<Leg> getLeg(int id);

		int id;
		Status status;
		OC oc;
		int link_id;

		time_t tradeDate;
		time_t settlementDate;
		string traderId;
		string userId;
		Side side;
		string symbol;
		double spotPrice;
		double riskFreeRate;
		double dividendYield;
		string strategy;

		time_t quoteStart;
		time_t quoteExpire;
		long quoteRemain;

		vector<shared_ptr<Leg>> legs;

		string toString(Status status);
		string toString(OC oc);
		OC toOC(string s);

	private:
		mutex lock;
};

}

#endif /* _conversation_h */
