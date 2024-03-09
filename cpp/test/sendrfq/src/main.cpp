#define BOOST_TEST_MAIN

#include "../../includes/log.hpp"

#include "../../mocks/src/MockDao.hpp"
#include "../../mocks/src/MockDaoFactory.hpp"
#include "../../mocks/src/MockWebSocketWriter.hpp"
#include "../../../src/AppCtx.hpp"
#include "../../../src/Dao.hpp"
#include "../../../src/DaoFactory.hpp"
#include "../../../src/Session.hpp"
#include "../../../src/SessionManager.hpp"
#include "../../../src/Conversation.hpp"
#include "../../../src/ConversationManager.hpp"
#include "../../../src/actions/SendRfq.hpp"

using namespace optquote;

class MyListener : public ConversationChangeListener {
	public :
		MyListener(string id) : id(id){}
		string id;
		void update(ConversationChangeEvent event){
			cout << "test... " << id << endl;
		}
};

BOOST_AUTO_TEST_CASE(test)
{
	init_log();
	BOOST_LOG_NAMED_SCOPE("main");
	BOOST_LOG_TRIVIAL(info) << "Info Log";

	AppCtx appCtx;
	MockDaoFactory daoFactory;
	ConversationManager conversationManager;
	SessionManager sessionManager;
	MyListener listener1{"1"}, listener2{"2"};

	appCtx.setSessionManager(&sessionManager);
	appCtx.setConversationManager(&conversationManager);
	appCtx.setDaoFactory(&daoFactory);

	string ss;
	unique_ptr<Document> req{new Document()};

	Document::AllocatorType& alloc = req->GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("send_rfq");
	Value sid;
	Value legs(kArrayType);
	Value leg1(kObjectType);
	Value leg2(kObjectType);

	string clientId{"clientId"};
	string clientPwd{"clientPwd"};
	string clientRole{"client"};
	string traderId{"traderId"};
	string traderPwd{"traderPwd"};
	string traderRole{"trader"};

  shared_ptr<Session> session = sessionManager.login(
      clientId,
      clientPwd,
      clientRole
      );
	session->writer = shared_ptr<WebSocketWriter>(new MockWebSocketWriter("1"));

  shared_ptr<Session> session2 = sessionManager.login(
      traderId,
      traderPwd,
      traderRole
      );
	session2->writer = shared_ptr<WebSocketWriter>(new MockWebSocketWriter("2"));

  const char* csid = session->id.c_str();
  sid.SetString(StringRef(csid));

	hdr.AddMember("type", type, alloc);
	hdr.AddMember("sessionId", sid, alloc);

	bdy.AddMember("symbol", "AAPL", alloc);
	bdy.AddMember("settlement", "T1", alloc);
	bdy.AddMember("side", "B", alloc);

	leg1.AddMember("ls", "L", alloc);
	leg1.AddMember("cp", "P", alloc);
	leg1.AddMember("quantity", 1000, alloc);
	leg1.AddMember("maturity", "2019-09-30", alloc);

	leg2.AddMember("ls", "S", alloc);
	leg2.AddMember("cp", "C", alloc);
	leg2.AddMember("quantity", 1000, alloc);
	leg2.AddMember("maturity", "2019-09-30", alloc);

	legs.PushBack(leg1, alloc);
	legs.PushBack(leg2, alloc);

	bdy.AddMember("legs", legs, alloc);

	req->SetObject()
		.AddMember("header", hdr, alloc)
		.AddMember("body", bdy, alloc);

	conversationManager.addSessionManager(&sessionManager);
	conversationManager.addListener(&listener1);
	conversationManager.addListener(&listener2);

	SendRfq action(&appCtx, session);

	action.doAction(move(req), ss);

	cout << "ss:" << ss << endl;

	BOOST_LOG_TRIVIAL(info) << "app exiting";
}


BOOST_AUTO_TEST_CASE(test_no_legs)
{
	init_log();
	BOOST_LOG_NAMED_SCOPE("main");
	BOOST_LOG_TRIVIAL(info) << "Info Log";

	AppCtx appCtx;
	MockDaoFactory daoFactory;
	ConversationManager conversationManager;
	SessionManager sessionManager;
	MyListener listener1{"1"}, listener2{"2"};

	appCtx.setSessionManager(&sessionManager);
	appCtx.setConversationManager(&conversationManager);
	appCtx.setDaoFactory(&daoFactory);

	string ss;
	unique_ptr<Document> req{new Document()};

	Document::AllocatorType& alloc = req->GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("send_rfq");
	Value sid;
	Value legs(kArrayType);

	string clientId{"clientId"};
	string clientPwd{"clientPwd"};
	string clientRole{"client"};
	string traderId{"traderId"};
	string traderPwd{"traderPwd"};
	string traderRole{"trader"};

  shared_ptr<Session> session = sessionManager.login(
			clientId,
			clientPwd,
			clientRole
      );
	session->writer = shared_ptr<WebSocketWriter>(new MockWebSocketWriter("1"));

  shared_ptr<Session> session2 = sessionManager.login(
      traderId,
      traderPwd,
      traderRole
      );
	session2->writer = shared_ptr<WebSocketWriter>(new MockWebSocketWriter("2"));

  const char* csid = session->id.c_str();
  //sid.SetString(csid, strlen(csid), alloc);
  sid.SetString(StringRef(csid));

	hdr.AddMember("type", type, alloc);
	hdr.AddMember("sessionId", sid, alloc);

	bdy.AddMember("symbol", "AAPL", alloc);
	bdy.AddMember("settlement", "T1", alloc);
	bdy.AddMember("side", "B", alloc);

	bdy.AddMember("legs", legs, alloc);

	req->SetObject()
		.AddMember("header", hdr, alloc)
		.AddMember("body", bdy, alloc);

	conversationManager.addSessionManager(&sessionManager);
	conversationManager.addListener(&listener1);
	conversationManager.addListener(&listener2);

	SendRfq action(&appCtx, session);

	action.doAction(move(req), ss);

	cout << "ss:" << ss << endl;

	BOOST_LOG_TRIVIAL(info) << "app exiting";
}


BOOST_AUTO_TEST_CASE(test_stress)
{
	init_log();
	BOOST_LOG_NAMED_SCOPE("main");
	BOOST_LOG_TRIVIAL(info) << "Info Log";

	AppCtx appCtx;
	MockDaoFactory daoFactory;
	ConversationManager conversationManager;
	SessionManager sessionManager;
	MyListener listener1{"1"}, listener2{"2"};

	appCtx.setSessionManager(&sessionManager);
	appCtx.setConversationManager(&conversationManager);
	appCtx.setDaoFactory(&daoFactory);

	string ss;
	unique_ptr<Document> req{new Document()};

	Document::AllocatorType& alloc = req->GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("send_rfq");
	Value sid;
	Value legs(kArrayType);

	string clientId{"clientId"};
	string clientPwd{"clientPwd"};
	string clientRole{"client"};
	string traderId{"traderId"};
	string traderPwd{"traderPwd"};
	string traderRole{"trader"};

  shared_ptr<Session> sesClient = sessionManager.login(
      clientId,
      clientPwd,
      clientRole
      );
	sesClient->writer = shared_ptr<WebSocketWriter>(new MockWebSocketWriter("1"));

  shared_ptr<Session> sesTrader = sessionManager.login(
      traderId,
      traderPwd,
      traderRole
      );
	sesTrader->writer = shared_ptr<WebSocketWriter>(new MockWebSocketWriter("2"));

  const char* csid = sesClient->id.c_str();
  sid.SetString(StringRef(csid));

	hdr.AddMember("type", type, alloc);
	hdr.AddMember("sessionId", sid, alloc);

	bdy.AddMember("symbol", "AAPL", alloc);
	bdy.AddMember("settlement", "T1", alloc);
	bdy.AddMember("side", "B", alloc);

	bdy.AddMember("legs", legs, alloc);

	req->SetObject()
		.AddMember("header", hdr, alloc)
		.AddMember("body", bdy, alloc);

	conversationManager.addSessionManager(&sessionManager);
	conversationManager.addListener(&listener1);
	conversationManager.addListener(&listener2);

	SendRfq sendRfq(&appCtx, sesClient);

	int mx = 1000000;
	int i = 0;
	for(;i < mx; i++){
		sendRfq.doAction(move(req), ss);
		BOOST_LOG_TRIVIAL(info) << "ss:" << ss << endl;
	}

  sessionManager.logout(sesClient);
  sessionManager.logout(sesTrader);

	BOOST_LOG_TRIVIAL(info) << "app exiting";
}
