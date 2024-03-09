
#include <iostream>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <syslog.h>
#include <memory>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/named_scope.hpp>

#include "../AppCtx.hpp"
#include "../Dao.hpp"
#include "GetSymbolListAction.hpp"

using namespace rapidjson;

namespace optquote {

GetSymbolListAction::GetSymbolListAction(AppCtx* appCtx) : appCtx(appCtx) {
}

GetSymbolListAction::~GetSymbolListAction() {
}

void GetSymbolListAction::doAction(unique_ptr<Document> rq, string& ss){
	vector<string> vsymbols;

	shared_ptr<Dao> dao = appCtx->getDaoFactory()->create();

	dao->getSymbolList(vsymbols);

	Document res;
	Document::AllocatorType& alloc = res.GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("symbols");

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);

	Value symbols(kArrayType);

	for(const string s : vsymbols){
		Value sym;
		sym.SetString(StringRef(s.c_str()));
		symbols.PushBack(sym, alloc);
	}
	bdy.AddMember("symbols", symbols, alloc);

	hdr.AddMember("type", type, alloc);

	res.SetObject()
		.AddMember("header", hdr, alloc)
		.AddMember("body", bdy, alloc);

	res.Accept(writer);

	ss.append(buffer.GetString(), buffer.GetLength());
}

}

