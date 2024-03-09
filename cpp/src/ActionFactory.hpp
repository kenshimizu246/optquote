
#include <string>
#include <exception>
#include <stdexcept>

#include <libwebsockets.h>
#include <rapidjson/document.h>

#include "actions/Action.hpp"
#include "AppCtx.hpp"

using namespace rapidjson;

namespace optquote {

class ActionFactory {
	private:

	public:
		ActionFactory();
		~ActionFactory();
		Action* getActionHandler(string& actionName, string& sessionId, AppCtx *appCtx);
};

}
