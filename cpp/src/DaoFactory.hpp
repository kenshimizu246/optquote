#include <string>
#include <sstream>
#include <vector>
#include <mutex>
#include <memory>

#include "Dao.hpp"

using namespace std;

#ifndef _daofactory_h
#define _daofactory_h

namespace optquote {

class DaoFactory {
	public:
		virtual shared_ptr<Dao> create() = 0;
};

}

#endif /* _daofactory_h */
