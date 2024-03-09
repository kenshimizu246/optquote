# define BOOST_TEST_MAIN

#include "../../includes/log.hpp"

#include <iostream>
#include <thread>

#include "../../mocks/src/MockDao.hpp"
#include "../../mocks/src/MockDaoFactory.hpp"
#include "../../mocks/src/MockWebSocketWriter.hpp"
#include "../../../src/AppCtx.hpp"
#include "../../../src/Dao.hpp"
#include "../../../src/DaoFactory.hpp"
#include "../../../src/ActionFactory.hpp"
#include "../../../src/Session.hpp"
#include "../../../src/SessionManager.hpp"
#include "../../../src/Conversation.hpp"
#include "../../../src/ConversationManager.hpp"
#include "../../../src/actions/SendRfq.hpp"
#include "../../../src/ActionExecutor.hpp"
#include "../../../src/Executor.hpp"

#include <libwebsockets.h>


using namespace std;
using namespace optquote;

BOOST_AUTO_TEST_CASE(test1)
{
	init_log();
	BOOST_LOG_NAMED_SCOPE("main");
	BOOST_LOG_TRIVIAL(info) << "Info Log";

	AppCtx appCtx;
	MockDaoFactory daoFactory;
	ConversationManager conversationManager;
	SessionManager sessionManager;
	ActionFactory actionFactory;

	appCtx.setSessionManager(&sessionManager);
	appCtx.setConversationManager(&conversationManager);
	appCtx.setDaoFactory(&daoFactory);

	string ss;
	shared_ptr<WebSocketWriter> writer{new MockWebSocketWriter("1")};

	for(int i = 0; i < 1000000; i++){
		unique_ptr<Document> rq{new Document()};

		Document::AllocatorType& alloc = rq->GetAllocator();
		Value hdr(kObjectType);
		Value bdy(kObjectType);
		Value type("action");
		Value action_name("heartbeat");
		Value message("hello");


		hdr.AddMember("type", type, alloc);
		hdr.AddMember("action_name", action_name, alloc);
		bdy.AddMember("message", message, alloc);

		rq->SetObject()
			.AddMember("header", hdr, alloc)
			.AddMember("body", bdy, alloc);

		string actionName{"heartbeat"};
		string sessionId{""};
	
		unique_ptr<Action> action{actionFactory.getActionHandler(actionName, sessionId, &appCtx)};

		unique_ptr<Executor> executor;
		executor.reset(new ActionExecutor(move(action), shared_ptr<WebSocketWriter>(writer), move(rq)));

		thread tt([px = move(executor)]{
			px->execute();
		});
		tt.join();

		BOOST_LOG_TRIVIAL(info) << "app thread done";
	}
	BOOST_LOG_TRIVIAL(info) << "app exiting";
}


