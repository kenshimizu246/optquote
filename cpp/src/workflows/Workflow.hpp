
#include <string>
#include <vector>
#include <mutex>
#include <exception>
#include <stdexcept>

#include "Session.hpp"

using namespace std;

#ifndef _workflow_h
#define _workflow_h

class Workflow {
	private:
		Workflow();
		~Workflow();
		mutex lock;

	public:
		void start();
};

#endif /* _workflow_h */

