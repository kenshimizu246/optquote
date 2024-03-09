#include <string>
#include <sstream>
#include <vector>
#include <mutex>
#include <memory>


#include "../../../src/Conversation.hpp"
#include "../../../src/Dao.hpp"

using namespace std;
using namespace optquote;

#ifndef _mockdao_h
#define _mockdao_h

class MockDao : public Dao{
	public:
		MockDao();
		~MockDao();

		int countCommit;
		int countRollback;
		int countClose;

		void commit(){
			countCommit++;
		}
		void rollback(){
			countRollback++;
		}
		void close(){
			countClose++;
		}

    void sendRfq(shared_ptr<Conversation> c);
    void takeOwnership(shared_ptr<Conversation> c);
    void quote(shared_ptr<Conversation> c);
    void timeoutQuote(shared_ptr<Conversation> c);
    void counter(shared_ptr<Conversation> c);
    void order(shared_ptr<Conversation> c);
    void execute(shared_ptr<Conversation> c);
    void chat(shared_ptr<Conversation> c, string& userId, string& message);
    void insert(shared_ptr<Conversation> c);
    void update(shared_ptr<Conversation> c);
    void load(int id, shared_ptr<Conversation> c);
    void load(vector<int>& ids, string& start, string& end);
    void load(vector<shared_ptr<Conversation>>& conversations, vector<int>& ids);
    void load(ConversationManager& cm);
    void load(SessionManager& sm);

    void getSymbolList(vector<string>& symbols);
    void getRfqList(vector<shared_ptr<Conversation>> rfqs);
    void getPriceList(vector<shared_ptr<Price>>& prices);

    unsigned int addLoginHistory(string& userId, string& role, string& sessionId);
    void addAccessHistory(shared_ptr<Session> session);
};

#endif /* _mockdao_h */
