#define BOOST_TEST_MAIN

#include "../../includes/log.hpp"

#include <iostream>
#include <sstream>
#include <memory>
#include <time.h>

#include "../../../src/Config.hpp"
#include "../../../src/AppCtx.hpp"
#include "../../../src/Dao.hpp"
#include "../../../src/DaoFactory.hpp"
#include "../../../src/MariaDbDaoFactory.hpp"
#include "../../../src/Price.hpp"
#include "../../../src/Session.hpp"
#include "../../../src/SessionManager.hpp"
#include "../../../src/ConversationManager.hpp"

using namespace std;
using namespace optquote;

BOOST_AUTO_TEST_CASE(test_simple)
{
	init_log();
	BOOST_LOG_NAMED_SCOPE("main");
	BOOST_LOG_TRIVIAL(info) << "Info Log";

	Config::getInstance().load("../../resources/config.json");

	AppCtx appCtx;
	MariaDbDaoFactory factory;

	shared_ptr<Dao> dao = factory.create();

	vector<string> symbols;
	dao->getSymbolList(symbols);
	for(string sym : symbols){
		BOOST_LOG_TRIVIAL(info) << "symbol:" << sym;
	}

	shared_ptr<Conversation> c = shared_ptr<Conversation>(new Conversation());

	dao->load(1, c);

	BOOST_LOG_TRIVIAL(info) << "loaded:" << c->id;


	vector<int> ii;
	string start{"2018-09-01"};
	string end{"2018-09-30"};
	dao->load(ii, start, end);

	for(int i : ii){
		BOOST_LOG_TRIVIAL(info) << "loaded i:" << i;
	}

	c->id = 99;
	dao->insert(c);

	vector<shared_ptr<Price>> prices;
	dao->getPriceList(prices);
	for(shared_ptr<Price> price : prices){
		char buff[20];
		strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&price->date));

		BOOST_LOG_TRIVIAL(info) << "symbol:" << price->symbol;
		BOOST_LOG_TRIVIAL(info) << "date:" << price->date;
		BOOST_LOG_TRIVIAL(info) << "date.t:" << buff;
		BOOST_LOG_TRIVIAL(info) << "price:" << price->price;
	}

	ConversationManager cnvMgr;
	dao->load(cnvMgr);

	SessionManager sm;
	dao->load(sm);
	BOOST_LOG_TRIVIAL(info) << "SessionManager.count:" << sm.count();

	BOOST_LOG_TRIVIAL(info) << "app exiting";
}




