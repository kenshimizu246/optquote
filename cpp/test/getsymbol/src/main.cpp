# define BOOST_TEST_MAIN

#include "../../includes/log.hpp"

#include <iostream>
#include <memory>

#include "../../mocks/src/MockDao.hpp"
#include "../../mocks/src/MockDaoFactory.hpp"
#include "../../../src/Dao.hpp"
#include "../../../src/DaoFactory.hpp"
#include "../../../src/Session.hpp"
#include "../../../src/SessionManager.hpp"
#include "../../../src/actions/GetSymbolListAction.hpp"

using namespace std;
using namespace optquote;

BOOST_AUTO_TEST_CASE(test1)
{
	init_log();
	BOOST_LOG_NAMED_SCOPE("main");
	BOOST_LOG_TRIVIAL(info) << "Info Log";

	AppCtx appCtx;
	SessionManager sessionManager;
	MockDaoFactory daoFactory;

	appCtx.setSessionManager(&sessionManager);
	appCtx.setDaoFactory(&daoFactory);

	string ss;
	unique_ptr<Document> req{new Document()};

	Document::AllocatorType& alloc = req->GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("get_symbol_list");
	Value sid;

	string clientId{"clientId"};
	string clientPwd{"clientPwd"};
	string clientRole{"client"};


  shared_ptr<Session> session = sessionManager.login(
			clientId,
			clientPwd,
			clientRole
      );

  const char* csid = session->id.c_str();
  sid.SetString(csid, strlen(csid), alloc);

	hdr.AddMember("type", type, alloc);
	hdr.AddMember("sessionId", sid, alloc);

	req->SetObject()
		.AddMember("header", hdr, alloc)
		.AddMember("body", bdy, alloc);

	GetSymbolListAction action(&appCtx);

	cout << "before doAction!" << endl;

	action.doAction(move(req), ss);

	cout << "ss:" << ss << endl;

	BOOST_LOG_TRIVIAL(info) << "app exiting";
}


