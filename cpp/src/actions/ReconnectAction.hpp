
#include <string>
#include <exception>
#include <stdexcept>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include "Action.hpp"
#include "../AppCtx.hpp"
#include "../Session.hpp"

using namespace std;
using namespace rapidjson;

namespace optquote {

class ReconnectAction 
		: public Action {
	private:
		AppCtx *appCtx;
		shared_ptr<Session> session;

	public:
		ReconnectAction(AppCtx *appCtx, shared_ptr<Session> session);
		~ReconnectAction();
		void doAction(unique_ptr<Document> rq, string& ss);
};

}


