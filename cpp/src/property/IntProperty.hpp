#include <string>
#include <sstream>
#include <vector>
#include <ql/quantlib.hpp>

#include "Property.hpp"

using namespace std;
using namespace QuantLib;

#ifndef int_property_hpp
#define int_property_hpp

class IntProperty : public Property<int> {
	public:
		IntProperty(string name) : Property(name){init();};
		IntProperty(IntProperty& p) : Property(p){init();};
		~IntProperty();
		static string toString(int v);
		int toValue(string s);
		int toValue(char *c);
		static void init();

		void setValue(string s);
		void setValue(char *c);
		using Property::setValue;
		string toString();

		IntProperty operator = (int i);
		IntProperty operator = (char* c);
		IntProperty operator = (string s);

	private:
		static bool initialized;
};

#endif /* int_property_hpp */
