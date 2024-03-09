
#include <sstream>
#include <vector>

#include "DaoFactory.hpp"
#include "SessionManager.hpp"
#include "ConversationManager.hpp"

using namespace std;

#ifndef _appctx_h
#define _appctx_h

namespace optquote {

class AppCtx {
	public:
		AppCtx(){};
		~AppCtx(){};

		void setSessionManager(SessionManager* m){
			sessionManager = m;
		}
		SessionManager* getSessionManager(){
			return sessionManager;
		}

		void setConversationManager(ConversationManager* c){
			conversationManager = c;
		}
		ConversationManager* getConversationManager(){
			return conversationManager;
		}

		void setDaoFactory(DaoFactory* m){
			daoFactory = m;
		}
		DaoFactory* getDaoFactory(){
			return daoFactory;
		}

	private:
		SessionManager* sessionManager;
		ConversationManager* conversationManager;
		DaoFactory* daoFactory;;
};

}

#endif /* _appctx_h */
