# define BOOST_TEST_MAIN

#include "../../includes/log.hpp"

#include <iostream>

#include "../../../src/SessionManager.hpp"

using namespace std;
using namespace optquote;

BOOST_AUTO_TEST_CASE(test1)
{
	init_log();
	BOOST_LOG_NAMED_SCOPE("main");
	BOOST_LOG_TRIVIAL(info) << "Info Log";

	SessionManager sessionManager;

	string adminUid{"admin"};
	string traderUid{"trader"};
	string clientUid{"client"};
	string passwd{"pwd"};
	string adminRole{"admin"};
	string traderRole{"trader"};
	string clientRole{"client"};

	shared_ptr<Session> sa = sessionManager.login(adminUid, passwd, adminRole);

	BOOST_CHECK(sessionManager.count() == 1);

	BOOST_CHECK(sa->role == adminRole);

	BOOST_CHECK(sa->valid);

	shared_ptr<Session> trader = sessionManager.login(traderUid, passwd, traderRole);

	shared_ptr<Session> client = sessionManager.login(clientUid, passwd, clientRole);

	BOOST_CHECK(sessionManager.count() == 3);

	sessionManager.logout(client);

	BOOST_CHECK(sessionManager.count() == 2);

	shared_ptr<Session> c1 = sessionManager.getSessionBySessionID(trader->id);
	shared_ptr<Session> c2 = sessionManager.getSessionByUserID(trader->userId);

	BOOST_CHECK(c1->id == c2->id);

	bool found = sessionManager.isValid(trader->id);

	BOOST_CHECK(found);

	found = sessionManager.isValid(client->id);

	BOOST_CHECK(!found);

	// needs memory check test.

	BOOST_LOG_TRIVIAL(info) << "app exiting";
}


BOOST_AUTO_TEST_CASE(test_memory_leak)
{
	init_log();
	BOOST_LOG_NAMED_SCOPE("main");
	BOOST_LOG_TRIVIAL(info) << "Info Log";

	SessionManager sessionManager;

	int maxcnt = 1000;
	int i = 0;
	for(; i < maxcnt; i++){
		string userId;
		string passwd{"pwd"};
		string role{"trader"};
		userId = string("user-") + to_string(i);
		shared_ptr<Session> sa = sessionManager.login(userId, passwd, role);
		sessionManager.logout(sa);
	}
	BOOST_LOG_TRIVIAL(info) << "app exiting";
}


