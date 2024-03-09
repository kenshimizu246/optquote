
#include <string>
#include <vector>
#include <mutex>
#include <exception>
#include <stdexcept>

#include "Session.hpp"

using namespace std;

#ifndef _task_h
#define _task_h

class Task {
	private:
		Task();
		~Task();
		mutex lock;

	public:
		void do();
};

#endif /* _task_h */

