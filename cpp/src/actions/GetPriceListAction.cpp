
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

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include "../AppCtx.hpp"
#include "../Dao.hpp"
#include "../Price.hpp"
#include "GetPriceListAction.hpp"

using namespace rapidjson;

namespace optquote {

GetPriceListAction::GetPriceListAction(AppCtx* appCtx) : appCtx(appCtx) {
}

GetPriceListAction::~GetPriceListAction() {
	syslog(LOG_INFO, "GetPriceListAction:delete:start");
	syslog(LOG_INFO, "GetPriceListAction:delete:end");
}

void GetPriceListAction::doAction(unique_ptr<Document> rq, string& ss){
	vector<shared_ptr<Price>> vprices;

	shared_ptr<Dao> dao = appCtx->getDaoFactory()->create();

	dao->getPriceList(vprices);

	Document res;
	Document::AllocatorType& alloc = res.GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("prices");

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);

	Value prices(kArrayType);

	for(const shared_ptr<Price> p : vprices){
		Value pp(kObjectType);
		Value sym;
		Value description;
		Value date;
		Value price;
		Value dividendYield;

		char buff[20];
		strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&p->date));

		sym.SetString(StringRef(p->symbol.c_str()));
		description.SetString(StringRef(p->description.c_str()));
		date.SetString(StringRef(buff));
		price.SetDouble(p->price);
		dividendYield.SetDouble(0.03);

		pp.AddMember("symbol", sym, alloc);
		pp.AddMember("description", description, alloc);
		pp.AddMember("date", date, alloc);
		pp.AddMember("price", price, alloc);
		pp.AddMember("dividendYield", dividendYield, alloc);
	
		prices.PushBack(pp, alloc);
	}
	bdy.AddMember("prices", prices, alloc);

	hdr.AddMember("type", type, alloc);

	res.SetObject()
		.AddMember("header", hdr, alloc)
		.AddMember("body", bdy, alloc);

	res.Accept(writer);

	ss.append(buffer.GetString(), buffer.GetLength());
}

}

