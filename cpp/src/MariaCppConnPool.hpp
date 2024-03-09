#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <mutex>
#include <pthread.h>
#include <thread>

#include <mariacpp/lib.hpp>
#include <mariacpp/connection.hpp>
#include <mariacpp/exception.hpp>
#include <mariacpp/prepared_stmt.hpp>
#include <mariacpp/time.hpp>
#include <mariacpp/uri.hpp>

#include "Config.hpp"

using namespace std;
using namespace MariaCpp;

#ifndef _mariadbpool_hpp
#define _mariadbpool_hpp

namespace optquote {

class ConnectionPool {
	public:
		ConnectionPool(string host, unsigned int port, string user, string passwd, string dbname, unsigned int min, unsigned int max);
		~ConnectionPool();
		Connection* getConnection();
		void releaseConnection(Connection* conn);

		static ConnectionPool& getInstance() {
			static ConnectionPool pool(
				Config::getInstance().getDbHost(),
				Config::getInstance().getDbPort(),
				Config::getInstance().getDbUser(),
				Config::getInstance().getDbPass(),
				Config::getInstance().getDbName(),
				2,
				5
			);
			return pool;
		}

	private:
		string host;
		unsigned int port;
		string user;
		string passwd;
		string dbname;
		unsigned int min;
		unsigned int max;
		unsigned int cntOpen;
		unsigned int cntLeased;
		thread *ping{nullptr};

		vector<Connection*> connections;
		mutex conns_mutex;

		void init();
		Connection* createConnection();
};

}

#endif /* _mariadbpool_hpp */

