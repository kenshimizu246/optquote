
#include <string>
#include <exception>
#include <stdexcept>

#include <libwebsockets.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include "Executor.hpp"
#include "WebSocketWriter.hpp"
#include "actions/Action.hpp"

using namespace std;
using namespace rapidjson;

namespace optquote {

class PThreadDetachException {
	public:
		int error;

		PThreadDetachException(int error);
};

class ActionExecutor : public Executor {
	private:
		unique_ptr<Action> action;
		shared_ptr<WebSocketWriter> writer;

	public:
		ActionExecutor(unique_ptr<Action> action, shared_ptr<WebSocketWriter> writer, unique_ptr<Document> rq);
		~ActionExecutor();
		void execute();
};

}
