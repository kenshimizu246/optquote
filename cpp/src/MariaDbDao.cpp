#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <mutex>
#include <memory>

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

#include "Dao.hpp"
#include "MariaDbDao.hpp"
#include "MariaCppConnPool.hpp"
#include "Conversation.hpp"
#include "Price.hpp"
#include "Session.hpp"

using namespace std;
using namespace MariaCpp;

namespace optquote {

MariaDbDao::MariaDbDao(){
	this->conn = ConnectionPool::getInstance().getConnection();
}

MariaDbDao::~MariaDbDao(){
	BOOST_LOG_TRIVIAL(info) << "MariaDbDao::~MariaDbDao():start...";
	ConnectionPool::getInstance().releaseConnection(conn);
	BOOST_LOG_TRIVIAL(info) << "MariaDbDao::~MariaDbDao():done...";
}

void MariaDbDao::getSymbolList(vector<string>& symbols){
	try {
		unique_ptr<PreparedStatement> stmt(
			conn->prepare("select symbol,description from symbol"));

		stmt->execute();
		while (stmt->fetch()) {
			cout << stmt->getString(0);
			cout << " : ";
			if (stmt->isNull(1)) cout << "NULL";
			else cout << stmt->getString(1);
			cout << endl;

			symbols.push_back(stmt->getString(0));
		}
	} catch (Exception &e) {
		cerr << e << endl;
	}
}

void MariaDbDao::getPriceList(vector<shared_ptr<Price>>& prices){
	try {
		// check error no.
		if(conn->errorno() != 0){
			throw runtime_error("connection error! " + string(conn->error_str()));
		}

		unique_ptr<PreparedStatement> stmt(
			conn->prepare("select symbol.symbol, symbol.description, price_v.date, price_v.price from price_v,symbol where date = (select max(date) from price_v) and symbol.symbol_id = price_v.symbol_id"));

		stmt->execute();
		while (stmt->fetch()) {
			string::size_type sz;
			double pp = stod(stmt->getString(3), &sz);

			Time t = stmt->getDate(2);

			time_t rawtime;
			struct tm * timeinfo;
			
			time(&rawtime);
			timeinfo = gmtime(&rawtime);
			timeinfo->tm_year = t.year - 1900;
			timeinfo->tm_mon = t.month;
			timeinfo->tm_mday = t.day;
			timeinfo->tm_hour = 0;
			timeinfo->tm_min = 0;
			timeinfo->tm_sec = 0;
			timeinfo->tm_isdst = 0;

			shared_ptr<Price> price = shared_ptr<Price>(new Price());

			price->symbol = stmt->getString(0);
			price->description = stmt->getString(1);
			price->date = mktime(timeinfo);
			price->price = pp;
			price->dividendYield = 0.03;

			prices.push_back(price);
		}
	} catch (Exception &e) {
		cerr << e << endl;
	}
}

void MariaDbDao::sendRfq(shared_ptr<Conversation> c){
	int id = 0;
	char buff[20];

	try {
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::sendRfq():start...";
		unique_ptr<PreparedStatement> stmt_id(
			conn->prepare("select nextval('conversation.id')"));

		stmt_id->execute();
		if(stmt_id->fetch()) {
			id = stmt_id->getInt(0);
		} else {
			// exception
		}
		c->id = id;

		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::sendRfq():id:" << id;

		unique_ptr<PreparedStatement> stmt(
			conn->prepare("insert into conversation(id, status_id, client_id, trade_date, settlement_date, symbol_id, strategy_id, spot_price, risk_free_rate, dividend_yield, oc_id, link_id) values(?,?,(select id from user_master where user_id = ?),?,?,(select symbol_id from symbol where symbol = ?), (select id from strategy where name = ?), ?, ?, ?, ?, ?)"));

		stmt->setInt(0, id); // id
		stmt->setInt(1, static_cast<int>(c->status)); // status_id
		stmt->setString(2, c->userId); // client_id
		strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&c->tradeDate));
		stmt->setString(3, buff); // trade_date
		strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&c->settlementDate));
		stmt->setString(4, buff); // settlement_date
		stmt->setString(5, c->symbol); // symbol_id
		stmt->setString(6, c->strategy); // strategy_id
		stmt->setDouble(7, c->spotPrice); // spot_price
		stmt->setDouble(8, c->riskFreeRate); // rate
		stmt->setDouble(9, c->dividendYield); // dividendYield
		stmt->setInt(10, static_cast<int>(c->oc)); // oc_id
		stmt->setInt(11, c->link_id); // link_id

		stmt->execute();

		unique_ptr<PreparedStatement> stmt_leg(
			conn->prepare("insert into leg(conv_id, leg_id, ls, cp, strike, quantity, maturity) values(?,?,?,?,?,?,?)"));
		for(shared_ptr<Leg> l : c->legs){
			stmt_leg->setInt(0, c->id);
			stmt_leg->setInt(1, l->id);
			stmt_leg->setChar(2, (l->ls == Long ? 'l' : 's'));
			stmt_leg->setChar(3, (l->cp == Call ? 'c' : 'p'));
			stmt_leg->setDouble(4, l->strike);
			stmt_leg->setDouble(5, l->quantity);
			strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&l->maturity));
			stmt_leg->setString(6, buff);
			stmt_leg->execute();
		}
		conn->commit();
	} catch (Exception &e) {
		cerr << e << endl;
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::sendRfq():exceptoin...:" << e;
		conn->rollback();
	}
	BOOST_LOG_TRIVIAL(info) << "MariaDbDao::sendRfq():end...";
}

void MariaDbDao::takeOwnership(shared_ptr<Conversation> c){
//	int id = 0;
	try {
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::takeOwnership():start...";

		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::takeOwnership():id:" << c->id;

		unique_ptr<PreparedStatement> stmt(
			conn->prepare("update conversation set status_id = ?, trader_id = (select id from user_master where user_id = ?) where id = ?"));

		stmt->setInt(0, static_cast<int>(c->status)); // status_id
		stmt->setString(1, c->traderId); // client_id
		stmt->setInt(2, c->id); // id

		stmt->execute();
		conn->commit();
	} catch (Exception &e) {
		conn->rollback();
		cerr << e << endl;
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::takeOwnership():exceptoin...:" << e;
	}
	BOOST_LOG_TRIVIAL(info) << "MariaDbDao::takeOwnership():end...";
}

void MariaDbDao::quote(shared_ptr<Conversation> c){
	char buff[20];
//	int id = 0;
	try {
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::quote():start...";

		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::quote():id:" << c->id;

		// check error no.
		if(conn->errorno() != 0){
			throw runtime_error("connection error! " + string(conn->error_str()));
		}

		unique_ptr<PreparedStatement> stmt(
			conn->prepare("update conversation set status_id = ?, trader_id = (select id from user_master where user_id = ?), quote_start = ?, quote_expire = ?, quote_remain = ? where id = ?"));

		unique_ptr<PreparedStatement> stmt_leg(
			conn->prepare("update leg set volatility = ?, premium = ?, delta = ?, gamma = ?, vega = ?, theta = ?, rho = ? where conv_id = ? and leg_id = ?"));

		for(shared_ptr<Leg> l: c->legs){
			BOOST_LOG_TRIVIAL(info) << "MariaDbDao::leg.id:" << c->id << ":" << l->id << ":" << l->premium;
			stmt_leg->setDouble(0, l->volatility);
			stmt_leg->setDouble(1, l->premium);
			stmt_leg->setDouble(2, l->delta);
			stmt_leg->setDouble(3, l->gamma);
			stmt_leg->setDouble(4, l->vega);
			stmt_leg->setDouble(5, l->theta);
			stmt_leg->setDouble(6, l->rho);
			stmt_leg->setInt(7, c->id);
			stmt_leg->setInt(8, l->id);
			stmt_leg->execute();
		}

		stmt->setInt(0, static_cast<int>(c->status)); // status_id
		stmt->setString(1, c->traderId); // client_id
		strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&c->quoteStart));
		stmt->setString(2, buff);
		strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&c->quoteExpire));
		stmt->setString(3, buff);
		stmt->setInt(4, c->quoteRemain);
		stmt->setInt(5, c->id); // id

		stmt->execute();
		conn->commit();
	} catch (Exception &e) {
		conn->rollback();
		cerr << e << endl;
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::quote():exceptoin...:" << e;
	}
	BOOST_LOG_TRIVIAL(info) << "MariaDbDao::quote():end...";
}

void MariaDbDao::counter(shared_ptr<Conversation> c){
//	int id = 0;
	try {
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::counter():start...";

		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::counter():id:" << c->id;

		// check error no.
		if(conn->errorno() != 0){
			throw runtime_error("connection error! " + string(conn->error_str()));
		}

		unique_ptr<PreparedStatement> stmt(
			conn->prepare("update conversation set status_id = ?, trader_id = (select id from user_master where user_id = ?) where id = ?"));

		unique_ptr<PreparedStatement> stmt_leg(
			conn->prepare("update leg set volatility = ?, premium = ?, delta = ?, gamma = ?, vega = ?, theta = ?, rho = ? where conv_id = ? and leg_id = ?"));

		for(shared_ptr<Leg> l: c->legs){
			BOOST_LOG_TRIVIAL(info) << "MariaDbDao::leg.id:" << c->id << ":" << l->id << ":" << l->premium;
			stmt_leg->setDouble(0, l->volatility);
			stmt_leg->setDouble(1, l->premium);
			stmt_leg->setDouble(2, l->delta);
			stmt_leg->setDouble(3, l->gamma);
			stmt_leg->setDouble(4, l->vega);
			stmt_leg->setDouble(5, l->theta);
			stmt_leg->setDouble(6, l->rho);
			stmt_leg->setInt(7, c->id);
			stmt_leg->setInt(8, l->id);
			stmt_leg->execute();
		}

		stmt->setInt(0, static_cast<int>(c->status)); // status_id
		stmt->setString(1, c->traderId); // client_id
		stmt->setInt(2, c->id); // id

		stmt->execute();
		conn->commit();
	} catch (Exception &e) {
		conn->rollback();
		cerr << e << endl;
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::counter():exceptoin...:" << e;
	}
	BOOST_LOG_TRIVIAL(info) << "MariaDbDao::counter():end...";
}

void MariaDbDao::order(shared_ptr<Conversation> c){
//	int id = 0;
	try {
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::order():start...";

		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::order():id:" << c->id;

		unique_ptr<PreparedStatement> stmt(
			conn->prepare("update conversation set status_id = ?, quote_remain = ?, trader_id = (select id from user_master where user_id = ?) where id = ?"));

		stmt->setInt(0, static_cast<int>(c->status)); // status_id
		stmt->setInt(1, c->quoteRemain); // status_id
		stmt->setString(2, c->traderId); // client_id
		stmt->setInt(3, c->id); // id

		stmt->execute();
		conn->commit();
	} catch (Exception &e) {
		conn->rollback();
		cerr << e << endl;
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::order():exceptoin...:" << e;
	}
	BOOST_LOG_TRIVIAL(info) << "MariaDbDao::order():end...";
}

void MariaDbDao::execute(shared_ptr<Conversation> c){
//	int id = 0;
	try {
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::execute():start...";

		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::execute():id:" << c->id;

		unique_ptr<PreparedStatement> stmt(
			conn->prepare("update conversation set status_id = ?, trader_id = (select id from user_master where user_id = ?) where id = ?"));

		stmt->setInt(0, static_cast<int>(c->status)); // status_id
		stmt->setString(1, c->traderId); // client_id
		stmt->setInt(2, c->id); // id

		stmt->execute();
		conn->commit();
	} catch (Exception &e) {
		conn->rollback();
		cerr << e << endl;
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::takeOwnership():exceptoin...:" << e;
	}
	BOOST_LOG_TRIVIAL(info) << "MariaDbDao::takeOwnership():end...";
}

void MariaDbDao::timeoutQuote(shared_ptr<Conversation> c){
	try {
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::timeoutQuote():start...";

		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::timeoutQuote():id:" << c->id;

		unique_ptr<PreparedStatement> stmt(
			conn->prepare("update conversation set status_id = ?, quote_remain = ? where id = ?"));

		stmt->setInt(0, static_cast<int>(c->status)); // status_id
		stmt->setInt(1, c->quoteRemain); // status_id
		stmt->setInt(2, c->id); // id

		stmt->execute();
		conn->commit();
	} catch (Exception &e) {
		conn->rollback();
		cerr << e << endl;
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::timeoutQuote():exceptoin...:" << e;
	}
	BOOST_LOG_TRIVIAL(info) << "MariaDbDao::timeoutQuote():end...";
}

void MariaDbDao::insert(shared_ptr<Conversation> c){
	int id = 0;
	try {
		unique_ptr<PreparedStatement> stmt_id(
			conn->prepare("select nextval('conversation.id')"));

		stmt_id->execute();
		if(stmt_id->fetch()) {
			id = stmt_id->getInt(0);
		} else {
			// exception
		}

		unique_ptr<PreparedStatement> stmt(
			conn->prepare("insert into conversation( conv_id, status_id, trader_id, client_id, trade_date, symbol_id, strategy_id, spot_price, rate) values(?,?,(select id from user_master where user_id = ?),(select id from user_master where user_id = ?),?,?,?,?,?)"));

		stmt->setInt(0, id); // status_id
		stmt->setInt(1, static_cast<int>(c->status)); // status_id
		stmt->setInt(1, 0); // trader_id
		stmt->setInt(2, 1); // client_id
		stmt->setString(3, "2018-09-27"); // trade_date
		stmt->setInt(4, 1); // symbol_id
		stmt->setInt(5, 1); // strategy_id
		stmt->setInt(6, 1); // spot_price
		stmt->setDouble(7, 0.01); // rate

		stmt->execute();
		conn->commit();
	} catch (Exception &e) {
		conn->rollback();
		cerr << e << endl;
	}
}

void MariaDbDao::chat(shared_ptr<Conversation> c, string& userId, string& message){

}

void MariaDbDao::update(shared_ptr<Conversation> c){
	try {
		unique_ptr<PreparedStatement> stmt(
			conn->prepare("update conversation set status_id = ?  , trader_id = (select id from user_master where user_id = ?) , client_id = (select id from user_master where user_id = ?) , trade_date = ?  , symbol_id = ?  , strategy_id = ?  , spot_price = ?  , rate = ?  where conv_id = ? "));

		stmt->setInt(0, static_cast<int>(c->status)); //   set status_id = ?
		stmt->setString(1, c->traderId); //     , trader_id = ?
		stmt->setString(2, c->userId); //     , client_id = ?
		stmt->setString(3, "2018-09-30"); //     , trade_date = ?
		stmt->setString(4, c->symbol); //     , symbol_id = ?
		stmt->setString(5, ""); //     , strategy_id = ?
		stmt->setDouble(6, 1); //     , spot_price = ?
		stmt->setDouble(7, 1); //     , rate = ?
		stmt->setInt(8, c->id); // where conv_id = ?

		stmt->execute();
		conn->commit();
	} catch (Exception &e) {
		conn->rollback();
		cerr << e << endl;
	}
}

void MariaDbDao::load(int id, shared_ptr<Conversation> c){
	try {
		unique_ptr<PreparedStatement> stmt(
			conn->prepare("select conv_id from conversation "));
		stmt->execute();
		if (stmt->fetch()) {
			c->id = stmt->getInt(0);
		}
	} catch (Exception &e) {
		cerr << e << endl;
	}
}

void MariaDbDao::load(vector<int>& ids, string& start, string& end){
	try {
		unique_ptr<PreparedStatement> stmt(
			conn->prepare("select conv_id from conversation "));
		stmt->execute();
		if (stmt->fetch()) {
			ids.push_back(stmt->getInt(0));
		}
	} catch (Exception &e) {
		cerr << e << endl;
	}
}

void MariaDbDao::load(vector<shared_ptr<Conversation>>& conversations, vector<int>& ids){
}

void MariaDbDao::load(ConversationManager& cm){
	try {
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::start to load into conversation manager";

		unique_ptr<PreparedStatement> stmt(
			conn->prepare("select c.id, c.status_id, c.trade_date, c.settlement_date, t.user_id, u.user_id, c.side_id, sym.symbol, c.spot_price, c.risk_free_rate, c.dividend_yield, s.name, c.oc_id, c.link_id, c.quote_start, c.quote_expire, c.quote_remain from conversation c left outer join user_master t on t.id = c.trader_id left outer join user_master u on u.id = c.client_id left outer join strategy s on s.id = c.strategy_id left outer join symbol sym on sym.symbol_id = c.symbol_id order by c.id desc limit 20 "));

		unique_ptr<PreparedStatement> stmt_leg(
			conn->prepare("select l.leg_id, l.ls, l.cp, l.strike, l.quantity, l.maturity, l.volatility, l.premium, l.delta, l.gamma, l.theta, l.vega, l.rho from leg l where conv_id = ? "));

		stmt->execute();
		while (stmt->fetch()) {
			time_t rawtime;
			struct tm * timeinfo;
			
			time(&rawtime);
			timeinfo = gmtime(&rawtime);
			timeinfo->tm_hour = 0;
			timeinfo->tm_min = 0;
			timeinfo->tm_sec = 0;
			timeinfo->tm_isdst = 0;

			shared_ptr<Conversation> c = shared_ptr<Conversation>(new Conversation());
			c->id = stmt->getInt(0);
			c->status =  static_cast<Status>(stmt->getInt(1));

			Time t = stmt->getDate(2);
			timeinfo->tm_year = t.year - 1900;
			timeinfo->tm_mon = t.month - 1;
			timeinfo->tm_mday = t.day;
			c->tradeDate = mktime(timeinfo);

			t = stmt->getDate(3);
			timeinfo->tm_year = t.year - 1900;
			timeinfo->tm_mon = t.month - 1;
			timeinfo->tm_mday = t.day;
			c->settlementDate = mktime(timeinfo);

			c->traderId = stmt->getString(4);
			c->userId = stmt->getString(5);
			c->side = static_cast<Side>(stmt->getInt(6));
			c->symbol = stmt->getString(7);

			string::size_type sz;
			double pp = stod(stmt->getString(8), &sz);
			c->spotPrice = pp;
			pp = stod(stmt->getString(9), &sz);
			c->riskFreeRate = pp;
			pp = stod(stmt->getString(10), &sz);
			c->dividendYield = pp;
			c->strategy = stmt->getString(11);
			c->oc = static_cast<OC>(stmt->getInt(12));
			c->link_id = stmt->getInt(13);

			t = stmt->getDate(14);
			timeinfo->tm_year = t.year - 1900;
			timeinfo->tm_mon = t.month - 1;
			timeinfo->tm_mday = t.day;
			c->quoteStart = mktime(timeinfo);

			t = stmt->getDate(15);
			timeinfo->tm_year = t.year - 1900;
			timeinfo->tm_mon = t.month - 1;
			timeinfo->tm_mday = t.day;
			c->quoteExpire = mktime(timeinfo);
			c->quoteRemain = stmt->getInt(16);

			stmt_leg->setInt(0, c->id);
			stmt_leg->execute();
			while (stmt_leg->fetch()) {
				shared_ptr<Leg> l = shared_ptr<Leg>(new Leg());
				l->id = stmt_leg->getInt(0); // l.leg_id
				//l->ls = static_cast<LS>(stmt_leg->getInt(1)); // l.ls
				if(stmt_leg->getString(1) == "s"){
					l->ls = Short;
				} else {
					l->ls = Long;
				}
				//l->cp = static_cast<CP>(stmt_leg->getInt(2)); // l.cp
				if(stmt_leg->getString(2) == "p"){
					l->cp = Put;
				} else {
					l->cp = Call;
				}
				pp = stod(stmt_leg->getString(3), &sz);
				l->strike = pp;
				pp = stod(stmt_leg->getString(4), &sz);
				l->quantity = pp;
				t = stmt_leg->getDate(5);
				timeinfo->tm_year = t.year - 1900;
				timeinfo->tm_mon = t.month - 1;
				timeinfo->tm_mday = t.day;
				l->maturity = mktime(timeinfo);
				pp = stod(stmt_leg->getString(6), &sz);
				l->volatility = pp;
				pp = stod(stmt_leg->getString(7), &sz);
				l->premium = pp;
				pp = stod(stmt_leg->getString(8), &sz);
				l->delta = pp;
				pp = stod(stmt_leg->getString(9), &sz);
				l->gamma = pp;
				pp = stod(stmt_leg->getString(10), &sz);
				l->theta = pp;
				pp = stod(stmt_leg->getString(11), &sz);
				l->vega = pp;
				pp = stod(stmt_leg->getString(12), &sz);
				l->rho = pp;
				c->legs.push_back(l);
				BOOST_LOG_TRIVIAL(info) << "MariaDbDao::adding leg:" << l->id;
			}
			cm.add(c);
			BOOST_LOG_TRIVIAL(info) << "MariaDbDao::load into conversation manager::" << c->id;
		}
	} catch (Exception &e) {
		cerr << e << endl;
	}
}

void MariaDbDao::getRfqList(vector<shared_ptr<Conversation>> rfqs){

}

unsigned int MariaDbDao::addLoginHistory(string& userId, string& role, string& sessionId){
	BOOST_LOG_TRIVIAL(info) << "MariaDbDao::addLoginHistory():[userId:" << userId
				<< "][role:" << role << "][sessionId:" << sessionId << "]";

	int id = -1;
	try {
    unique_ptr<PreparedStatement> stmt_id(
      conn->prepare("select nextval('login_history.id')"));

		stmt_id->execute();
		if(stmt_id->fetch()) {
			id = stmt_id->getInt(0);
		} else {
			// exception
		}

		unique_ptr<PreparedStatement> stmt(
			conn->prepare("insert into login_history(id, user_id, role, session_id) values(?,(select id from user_master where user_id = ?),?,?)"));

		stmt->setInt(0, id);
		stmt->setString(1, userId);
		stmt->setString(2, role);
		stmt->setString(3, sessionId);
		stmt->execute();

		unique_ptr<PreparedStatement> stmt_acc(
			conn->prepare("insert into access_history (id, login_history_id) values(nextval('access_history.id'), ?)"));

		stmt_acc->setInt(0, id);
		stmt_acc->execute();

		conn->commit();
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::addLoginHistory():commit successfully";
	} catch (Exception &e) {
		conn->rollback();
		BOOST_LOG_TRIVIAL(error) << "MariaDbDao::addLoginHistory():exception:" << e.what();
	} catch (...) {
		conn->rollback();
		BOOST_LOG_TRIVIAL(error) << "MariaDbDao::addLoginHistory():exception:...";
	}
	return id;
}

void MariaDbDao::addAccessHistory(shared_ptr<Session> session){
	try {
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::addAccessHitory():start...";
		unique_ptr<PreparedStatement> stmt(
			conn->prepare("insert into access_history (id, login_history_id) values(nextval('access_history.id'), (select max(id) from login_history where session_id = ?))"));

		stmt->setString(0, session->id);

		stmt->execute();
		conn->commit();
	} catch (Exception &e) {
		cerr << e << endl;
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::addAccessHistory():exceptoin...:" << e;
		conn->rollback();
	}
	BOOST_LOG_TRIVIAL(info) << "MariaDbDao::addAccessHistory():end...";
}

void MariaDbDao::load(SessionManager& sm){
	BOOST_LOG_TRIVIAL(info) << "MariaDbDao::load(SessionManager):start...";
	try {
		unique_ptr<PreparedStatement> stmt(
			conn->prepare("select t2.user_id, t0.role, t0.login_time, t0.session_id, t2.name from login_history t0, (select max(id) as id, user_id, role from login_history group by user_id, role) t1, user_master t2 where t0.id = t1.id and t2.id = t0.user_id"));

		stmt->execute();
		while (stmt->fetch()) {
			tm t;
			time_t lt;
			// string strDateTime{stmt->getString(2)};
			strptime(stmt->getString(2).c_str(), "%F %T", &t);
			lt = mktime(&t);

			BOOST_LOG_TRIVIAL(info) << "user_id:" << stmt->getString(0)
				<< ", role:" << stmt->getString(1)
				<< ", login_time:" << stmt->getString(2)
				<< ", session_id:" << stmt->getString(3);

			shared_ptr<Session> s = shared_ptr<Session>(new Session(
				stmt->getString(3),
				stmt->getString(0),
				stmt->getString(4),
				stmt->getString(1),
				true,
				lt
			));
			sm.add(s);
		}
		BOOST_LOG_TRIVIAL(info) << "MariaDbDao::load().executed";
	} catch (Exception &e) {
		cerr << e << endl;
	}
	BOOST_LOG_TRIVIAL(info) << "MariaDbDao::load():end...";
}

void MariaDbDao::commit(){
	conn->commit();
}

void MariaDbDao::rollback(){
	conn->rollback();
}

}


