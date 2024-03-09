
#ifndef _sessionmanager_h
#define _sessionmanager_h

#include <string>
#include <vector>
#include <mutex>
#include <map>
#include <exception>
#include <stdexcept>
#include <memory>

#include "Session.hpp"
#include "Conversation.hpp"
#include "ConversationManager.hpp"

using namespace std;

namespace optquote {

class SessionManager : public ConversationChangeListener {
	private:
		map<string, shared_ptr<Session>> sessions;
		map<string, shared_ptr<Session>> sessionByUserId;
		mutex mtx;
		int maxSession = 100;

	public:
		SessionManager();
		~SessionManager();
		shared_ptr<Session> login(string& userId, string& passwd, string& role);
		void logout(string& sessionId);
		shared_ptr<Session> getSessionByUserID(string userId);
		shared_ptr<Session> getSessionBySessionID(string id);
		int count();
		bool isValid(string sessionId);
		void update(ConversationChangeEvent event);
		void invalidate(time_t& t);
		void clean();
		void logout(shared_ptr<Session> s);
		void add(shared_ptr<Session> s);
		void setMaxSession(int cnt);
		int getMaxSession();

		// timeout
};

}

#endif /* _sessionmanager_h */

