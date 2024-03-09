#include <string>
#include <sstream>
#include <vector>
#include <ql/quantlib.hpp>

#include "Property.hpp"

using namespace std;
using namespace QuantLib;

#ifndef double_property_hpp
#define double_property_hpp

class DoubleProperty : public Property<double> {
	public:
		DoubleProperty(string name) : Property(name){init();};
		~DoubleProperty();
		static string toString(double v);
		double toValue(string s);
		double toValue(char *c);
		static void init();

		void setValue(string s);
		void setValue(char *c);
		using Property::setValue;
		string toString();

    DoubleProperty operator = (double d);
    DoubleProperty operator = (char* c);
    DoubleProperty operator = (string s);

	private:
		static bool initialized;
};

#endif /* double_property_hpp */
