
#include <string>
#include <exception>
#include <stdexcept>

#include <libwebsockets.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include "Executor.hpp"
#include "AppWebSocketWriter.hpp"
#include "session.h"

using namespace std;
using namespace rapidjson;

namespace optquote {

class LoginExecutor : public Executor  {
	public:
		LoginExecutor(shared_ptr<AppWebSocketWriter> writer, struct per_session_data_quote* pss, unique_ptr<Document> rq);
		~LoginExecutor();

		void execute();
	private:
		struct per_session_data_quote* pss;
		shared_ptr<AppWebSocketWriter> writer;
};

}
