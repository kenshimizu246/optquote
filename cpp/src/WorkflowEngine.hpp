
#include <string>
#include <vector>
#include <mutex>
#include <exception>
#include <stdexcept>

#include "Session.hpp"

using namespace std;

#ifndef _workflowengine_h
#define _workflowengine_h

namespace optquote {

class WrokflowEngine {
	private:
		WrokflowEngine();
		~SessionManager();
		mutex lock;

	public:
};

}

#endif /* _workflowengine_h */

