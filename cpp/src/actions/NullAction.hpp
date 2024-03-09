
#include <string>
#include <exception>
#include <stdexcept>

#include <libwebsockets.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include "Action.hpp"
#include "../AppCtx.hpp"

using namespace std;
using namespace rapidjson;

namespace optquote {

class NullAction 
		: public Action {
	private:

	public:
		NullAction();
		~NullAction();
		void doAction(unique_ptr<Document> rq, string& ss);
};

}

