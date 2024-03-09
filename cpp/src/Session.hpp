#include <sstream>
#include <time.h>
#include <memory>

#include "WebSocketWriter.hpp"

using namespace std;

#ifndef _session_h
#define _session_h

namespace optquote {

class Session {
	public:
		Session(string id, string userId, string userName, string role, bool valid);
		Session(string id, string userId, string userName, string role, bool valid, time_t start_time);
		~Session();
		void update();

		string id;
		string userId;
		string userName;
		string role;
		bool valid;
		time_t start_time;
		time_t last_accessed;
		unsigned int login_history_id;

		shared_ptr<WebSocketWriter> writer;
};

}

#endif /* _session_h */
