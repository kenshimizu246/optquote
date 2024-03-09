#include <string>
#include <sstream>
#include <vector>
#include <mutex>
#include <mysql.h>

#include <mariacpp/lib.hpp>
#include <mariacpp/connection.hpp>
#include <mariacpp/exception.hpp>
#include <mariacpp/prepared_stmt.hpp>
#include <mariacpp/time.hpp>
#include <mariacpp/uri.hpp>

#include "Dao.hpp"
#include "DaoFactory.hpp"
#include "MariaCppConnPool.hpp"

using namespace std;
using namespace MariaCpp;

#ifndef _mariadbdaofactory_h
#define _mariadbdaofactory_h

namespace optquote {

class MariaDbDaoFactory : public DaoFactory {
	public:
		MariaDbDaoFactory();
		~MariaDbDaoFactory();

		shared_ptr<Dao> create();
};

}

#endif /* _mariadbdaofactory_h */
