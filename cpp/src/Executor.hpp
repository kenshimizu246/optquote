
#include <string>
#include <exception>
#include <stdexcept>

#include <libwebsockets.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

using namespace std;
using namespace rapidjson;

#ifndef HEADERFILE_EXECUTOR
#define HEADERFILE_EXECUTOR

namespace optquote {

class Executor {
	private:

	public:
		unique_ptr<Document> rq;

		Executor(unique_ptr<Document> rq);
		virtual ~Executor();

		virtual void execute() = 0;
};

}

#endif // HEADERFILE_EXECUTOR
