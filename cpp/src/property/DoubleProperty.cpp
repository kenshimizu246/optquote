#include <string>
#include <sstream>
#include <vector>
#include <ql/quantlib.hpp>

#include "Property.hpp"
#include "DoubleProperty.hpp"

using namespace std;
using namespace QuantLib;

bool DoubleProperty::initialized = false;

DoubleProperty::~DoubleProperty(){
#ifdef _DEBUG_PROPERTIES
	cout << "~DoubleProperty(): start destruction!" << " : " << getName() << endl;
	cout << "~DoubleProperty(): end destruction!" << " : " << getName() << endl;
#endif
}

void DoubleProperty::init(){
#ifdef _DEBUG_PROPERTIES
	cout << "start void DoubleProperty::init(){" << endl;
#endif

	if (!initialized) {
		initialized = true;
	}

#ifdef _DEBUG_PROPERTIES
	cout << "end void DoubleProperty::init(){" << endl;
#endif
}

string DoubleProperty::toString(double v){
#ifdef _DEBUG_PROPERTIES
	cout << "string DoubleProperty::toString(Double [" << v << "])" << endl;
#endif
	return to_string(v);
}

double DoubleProperty::toValue(string s){
#ifdef _DEBUG_PROPERTIES
	cout << "int DoubleProperty::toValue(string [" << s << "])" << endl;
#endif
	return stod(s);
}

double DoubleProperty::toValue(char *c){
#ifdef _DEBUG_PROPERTIES
	cout << "int DoubleProperty::toValue(char *[" << c << "])" << endl;
#endif
	return stod(string(c));
}

void DoubleProperty::setValue(string s){
#ifdef _DEBUG_PROPERTIES
	cout << "void DoubleProperty::setValue(string [" << s << "])" << " : " << getName() << endl;
#endif
	setValue(toValue(s));
}

void DoubleProperty::setValue(char *c){
#ifdef _DEBUG_PROPERTIES
	cout << "void DoubleProperty::setValue(char *[" << c << "])" << " : " << getName() << endl;
#endif
	setValue(toValue(c));
}

string DoubleProperty::toString(){
#ifdef _DEBUG_PROPERTIES
	cout << "string DoubleProperty::toString() : " << getValue() << " : " << getName() << endl;
#endif
	return toString(getValue());
}

DoubleProperty DoubleProperty::operator = (double d){
  setValue(d);
  return *this;
}

DoubleProperty DoubleProperty::operator = (string s){
  setValue(s);
  return *this;
}

DoubleProperty DoubleProperty::operator = (char* c){
  setValue(c);
  return *this;
}






