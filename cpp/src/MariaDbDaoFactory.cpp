#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <mutex>
#include <mysql.h>
#include <memory>

#include <mariacpp/lib.hpp>
#include <mariacpp/connection.hpp>
#include <mariacpp/exception.hpp>
#include <mariacpp/prepared_stmt.hpp>
#include <mariacpp/time.hpp>
#include <mariacpp/uri.hpp>

#include "Dao.hpp"
#include "DaoFactory.hpp"
#include "MariaCppConnPool.hpp"
#include "MariaDbDao.hpp"
#include "MariaDbDaoFactory.hpp"

using namespace std;
using namespace MariaCpp;

namespace optquote {

MariaDbDaoFactory::MariaDbDaoFactory(){

}

MariaDbDaoFactory::~MariaDbDaoFactory(){
}

shared_ptr<Dao> MariaDbDaoFactory::create(){
	return shared_ptr<Dao>(new MariaDbDao());
}

}

