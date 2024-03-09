#ifndef _conv_mgr_h
#define _conv_mgr_h

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <map>

#include "Leg.hpp"
#include "Conversation.hpp"

using namespace std;

namespace optquote {

class SessionManager;

class ConversationChangeEvent {
	public:
		ConversationChangeEvent(shared_ptr<Conversation> c) : conversation(c) {};
		shared_ptr<Conversation> getConversation(){
			return conversation;
		}
	private:
		shared_ptr<Conversation> conversation;
};

class ConversationChangeListener {
	public:
		virtual void update(ConversationChangeEvent event){}
};

class ConversationManager {
	public:
		ConversationManager();
		~ConversationManager();

		void add(shared_ptr<Conversation> c);
		void remove(shared_ptr<Conversation> c);
		void sendConversationEvent(shared_ptr<Conversation> c);
		shared_ptr<Conversation> getConversation(int conversationId);
		void getMyConversations(vector<shared_ptr<Conversation>>& cc, string& userId);
		void checkTimeout();
		void doHousekeeping();

		void addListener(ConversationChangeListener* l){
			listeners.push_back(l);
		}
		void remove(ConversationChangeListener* l){
//			listeners.erase(remove(listeners.begin(), listeners.end(), l), listeners.end());
		}

		void addSessionManager(SessionManager* sessionManager){
			this->sessionManager = sessionManager;
		}

	private:
		mutex mtx;
		map<int, shared_ptr<Conversation>> conversations;
		vector<ConversationChangeListener*> listeners;
		void fireChangeListeners(ConversationChangeEvent event);
		SessionManager* sessionManager;
};

}

#endif /* _conv_mgr_h */
