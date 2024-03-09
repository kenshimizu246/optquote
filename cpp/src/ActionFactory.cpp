
#include <string>
#include <exception>
#include <stdexcept>
#include <syslog.h>

#include <rapidjson/document.h>

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

#include "AppCtx.hpp"
#include "SessionManager.hpp"
#include "ActionFactory.hpp"
#include "actions/Action.hpp"
#include "actions/NullAction.hpp"
#include "actions/ErrorAction.hpp"
#include "actions/GetSymbolListAction.hpp"
#include "actions/GetPriceListAction.hpp"
#include "actions/GetMyConversations.hpp"
#include "actions/TakeOwnershipAction.hpp"
#include "actions/CalcAction.hpp"
#include "actions/QuoteAction.hpp"
#include "actions/OrderAction.hpp"
#include "actions/ExecuteAction.hpp"
#include "actions/RejectRfqAction.hpp"
#include "actions/RejectQuoteAction.hpp"
#include "actions/RejectOrderAction.hpp"
#include "actions/CounterQuoteAction.hpp"
#include "actions/ReconnectAction.hpp"

#include "actions/HeartbeatAction.hpp"
#include "actions/SendRfq.hpp"

using namespace std;
using namespace rapidjson;

namespace optquote {

ActionFactory::ActionFactory(){
}

ActionFactory::~ActionFactory(){
}

Action* ActionFactory::getActionHandler(string& actionName, string& sessionId, AppCtx *appCtx){
	const string heartbeat("heartbeat");
	const string login("login");
	const string reconnect("reconnect");

	const string get_symbol_list("get_symbol_list");
	const string get_price_list("get_price_list");
	const string get_my_conversations("get_my_conversations");

	const string send_rfq("send_rfq");
	const string take_ownership("take_ownership");
	const string calc_option("calc_option");
	const string reject_rfq("reject_rfq");
	const string quote("quote");
	const string reject_quote("reject_quote");
	const string counter_quote("counter_quote");
	const string order("order");
	const string reject_order("reject_order");
	const string execute("execute");

	if (actionName == heartbeat) {
		return new HeartbeatAction(appCtx);
	} else {
		shared_ptr<Session> session = appCtx->getSessionManager()->getSessionBySessionID(sessionId);
		if(session == nullptr){
			return new ErrorAction(string("no session available!"));
		}
		if (actionName == get_symbol_list) {
			return new GetSymbolListAction(appCtx);
		} else if (actionName == get_price_list) {
			return new GetPriceListAction(appCtx);
		} else if (actionName == get_my_conversations) {
			return new GetMyConversations(appCtx, session);
		} else if (actionName == send_rfq) {
			return new SendRfq(appCtx, session);
		} else if (actionName == take_ownership) {
			return new TakeOwnershipAction(appCtx, session);
		} else if (actionName == calc_option) {
			return new CalcAction(appCtx, session);
		} else if (actionName == quote) {
			return new QuoteAction(appCtx, session);
		} else if (actionName == reject_rfq) {
			return new RejectRfqAction(appCtx, session);
		} else if (actionName == order) {
			return new OrderAction(appCtx, session);
		} else if (actionName == reject_quote) {
			return new RejectQuoteAction(appCtx, session);
		} else if (actionName == counter_quote) {
			return new CounterQuoteAction(appCtx, session);
		} else if (actionName == reject_order) {
			return new RejectOrderAction(appCtx, session);
		} else if (actionName == execute) {
			return new ExecuteAction(appCtx, session);
		} else if (actionName == reconnect) {
			return new ReconnectAction(appCtx, session);
		}
	}
	return new NullAction();
}

}

