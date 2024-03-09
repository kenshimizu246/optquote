#include <string>
#include <sstream>
#include <vector>
#include <mutex>

#include "Conversation.hpp"
#include "ConversationManager.hpp"
#include "Session.hpp"
#include "SessionManager.hpp"
#include "Price.hpp"

using namespace std;

#ifndef _dao_h
#define _dao_h

namespace optquote {

class Dao {
	public:
		virtual ~Dao(){};
		virtual void sendRfq(shared_ptr<Conversation> c) = 0;
		virtual void takeOwnership(shared_ptr<Conversation> c) = 0;
		virtual void quote(shared_ptr<Conversation> c) = 0;
		virtual void timeoutQuote(shared_ptr<Conversation> c) = 0;
		virtual void counter(shared_ptr<Conversation> c) = 0;
		virtual void order(shared_ptr<Conversation> c) = 0;
		virtual void execute(shared_ptr<Conversation> c) = 0;
		virtual void chat(shared_ptr<Conversation> c, string& userId, string& message) = 0;
		virtual void insert(shared_ptr<Conversation> c) = 0;
		virtual void update(shared_ptr<Conversation> c) = 0;
		virtual void load(int id, shared_ptr<Conversation> c) = 0;
		virtual void load(vector<int>& ids, string& start, string& end) = 0;
		virtual void load(vector<shared_ptr<Conversation>>& conversations, vector<int>& ids) = 0;
		virtual void load(ConversationManager& cm) = 0;
		virtual void load(SessionManager& sm) = 0;

		virtual void getSymbolList(vector<string>& symbols) = 0;
		virtual void getRfqList(vector<shared_ptr<Conversation>> rfqs) = 0;
		virtual void getPriceList(vector<shared_ptr<Price>>& prices) = 0;

		// virtual boolean validUser(string& userId, string& passwd, string& role) = 0;
		virtual unsigned int addLoginHistory(string& userId, string& role, string& sessionId) = 0;
		virtual void addAccessHistory(shared_ptr<Session> session) = 0;

		virtual void commit() = 0;
		virtual void rollback() = 0;
};

}

#endif /* _dao_h */
