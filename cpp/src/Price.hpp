#include <time.h>
#include <string>
#include <sstream>
#include <vector>
#include <mutex>

#include <rapidjson/document.h>

using namespace std;
using namespace rapidjson;

#ifndef _price_h
#define _price_h

namespace optquote {

class Price {
	public:
		Price();
		Price(string symbol, string description, time_t date, double price, double dividendYield);
		~Price();

		void toJsonString(string& ss);

		string symbol;
		string description;
		time_t date;
		double price;
		double dividendYield;
};

}

#endif /* _price_h */
