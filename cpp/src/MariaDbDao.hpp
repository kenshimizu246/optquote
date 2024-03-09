#include <sstream>
#include <vector>
#include <mutex>
#include <mysql.h>
#include <memory>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include <mariacpp/lib.hpp>
#include <mariacpp/connection.hpp>
#include <mariacpp/exception.hpp>
#include <mariacpp/prepared_stmt.hpp>
#include <mariacpp/time.hpp>
#include <mariacpp/uri.hpp>

#include "Dao.hpp"
#include "MariaCppConnPool.hpp"
#include "Price.hpp"
#include "ConversationManager.hpp"
#include "SessionManager.hpp"

using namespace std;
using namespace rapidjson;
using namespace MariaCpp;

#ifndef _mariadb_dao_h
#define _mariadb_dao_h

namespace optquote {

class MariaDbDao : public Dao {
	public:
		MariaDbDao();
		~MariaDbDao();

		void getSymbolList(vector<string>& symbols);
		void getPriceList(vector<shared_ptr<Price>>& prices);
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
    virtual void load(int id, shared_ptr<Conversation> c);
    virtual void load(vector<int>& ids, string& start, string& end);
    virtual void load(vector<shared_ptr<Conversation>>& conversations, vector<int>& ids);
    virtual void load(ConversationManager& cm);
    virtual void load(SessionManager& sm);
		void getRfqList(vector<shared_ptr<Conversation>> rfqs);

		unsigned int addLoginHistory(string& userId, string& role, string& sessionId);
		void addAccessHistory(shared_ptr<Session> session);

		void commit();
		void rollback();
		void close();

	private:
		Connection *conn;
};

}

#endif /* _mariadb_dao_h */
