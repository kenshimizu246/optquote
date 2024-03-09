#include <string>
#include <sstream>
#include <vector>
#include <mutex>
#include <ctime>

using namespace std;

#ifndef _chat_h
#define _chat_h

namespace optquote {

class Chat {
	public:
		Chat(string userId, string message);
		~Chat();

		int id;

	private:
};

}

#endif /* _chat_h */
