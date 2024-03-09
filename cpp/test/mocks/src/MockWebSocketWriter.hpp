#include <string>
#include <sstream>
#include <vector>
#include <mutex>

#include "../../../src/WebSocketWriter.hpp"

using namespace std;
using namespace optquote;

#ifndef _mock_websocket_writer_h
#define _mock_websocket_writer_h

class MockWebSocketWriter : public WebSocketWriter{
	public:
		MockWebSocketWriter(string id);
		~MockWebSocketWriter();

		string id;
		int write(string& ss);
};

#endif /* _mock_websocket_writer_h */
