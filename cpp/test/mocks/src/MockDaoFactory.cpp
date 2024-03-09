#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <mutex>

#include "../../../src/Dao.hpp"
#include "../../../src/DaoFactory.hpp"
#include "MockDao.hpp"
#include "MockDaoFactory.hpp"

using namespace std;
using namespace optquote;

MockDaoFactory::MockDaoFactory(){
}

MockDaoFactory::~MockDaoFactory(){
}

shared_ptr<Dao> MockDaoFactory::create(){
	return shared_ptr<Dao>(new MockDao());
}



