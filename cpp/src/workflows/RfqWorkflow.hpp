
#include <string>
#include <vector>
#include <mutex>
#include <exception>
#include <stdexcept>

#include "Session.hpp"

using namespace std;

#ifndef _rfqworkflow_h
#define _rfqworkflow_h

class RfqWrokflow {
	private:
		RfqWrokflow();
		~RfqWorkflow();
		mutex lock;

	public:
};

#endif /* _rfqworkflow_h */

