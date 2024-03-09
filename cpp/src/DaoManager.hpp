#include <string>
#include <sstream>
#include <vector>
#include <mutex>

#include "Dao.hpp"

using namespace std;

#ifndef _daomanager_h
#define _daomanager_h

namespace optquote {

class DaoManager {
	public:
		virtual Dao* get() = 0;
		virtual void release(Dao* dao) = 0;
};

}

#endif /* _daomanager_h */
