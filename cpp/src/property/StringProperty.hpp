#include <string>
#include <sstream>
#include <vector>
#include <ql/quantlib.hpp>

#include "Property.hpp"

using namespace std;
using namespace QuantLib;

#ifndef string_property_hpp
#define string_property_hpp

class StringProperty : public Property<string> {
	public:
		StringProperty(string name) : Property(name){init();};
		StringProperty(string name, string initValue) : Property(name, initValue){init();};
		StringProperty(StringProperty& p) : Property(p){init();};
		~StringProperty();
		static string toString(string v);
		string toValue(string s);
		string toValue(char *c);
		static void init();

//		void setValue(string s);
		void setValue(char *c);
		using Property::setValue;
		string toString();

		StringProperty operator = (char* c);
		StringProperty operator = (string s);

	private:
		static bool initialized;
};

#endif /* string_property_hpp */
