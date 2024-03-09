#include <time.h>
#include <string>

#include <iostream>
#include <sstream>
#include <vector>
#include <mutex>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include "Price.hpp"

namespace optquote {

Price::Price(){
}

Price::Price(string symbol, string description, time_t date, double price, double dividendYield):symbol(symbol), description(description), date(date), price(price), dividendYield(dividendYield){
}

Price::~Price() {
}

void Price::toJsonString(string& ss){
	Document res;
	Document::AllocatorType& alloc = res.GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);
	Value type("price");

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);

	Value pp(kObjectType);
	Value sym;
	Value desc;
	Value dt;
	Value prc;
	Value dividendYield;

	char buff[20];
	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&date));

	sym.SetString(StringRef(symbol.c_str()));
	desc.SetString(StringRef(description.c_str()));
	dt.SetString(StringRef(buff));
	prc.SetDouble(price);
	dividendYield.SetDouble(0.03);

	pp.AddMember("symbol", sym, alloc);
	pp.AddMember("description", desc, alloc);
	pp.AddMember("date", dt, alloc);
	pp.AddMember("price", prc, alloc);

	pp.AddMember("dividendYield", dividendYield, alloc);

	bdy.AddMember("price", pp, alloc);
	hdr.AddMember("type", type, alloc);

	res.SetObject()
		.AddMember("header", hdr, alloc)
		.AddMember("body", bdy, alloc);

	res.Accept(writer);

	ss.append(buffer.GetString(), buffer.GetLength());
}

}



