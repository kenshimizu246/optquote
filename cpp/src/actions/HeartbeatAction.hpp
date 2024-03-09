
#include <string>
#include <exception>
#include <stdexcept>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include "Action.hpp"
#include "../AppCtx.hpp"

using namespace std;
using namespace rapidjson;

namespace optquote {

class HeartbeatAction 
		: public Action {
	private:
		AppCtx *appCtx;

	public:
		HeartbeatAction(AppCtx *appCtx);
		~HeartbeatAction();
		void doAction(unique_ptr<Document> rq, string& ss);
};

}


