#include <string>
#include <sstream>
#include <vector>
#include <ql/quantlib.hpp>

#include "Property.hpp"
#include "StringProperty.hpp"

using namespace std;
using namespace QuantLib;

bool StringProperty::initialized = false;

StringProperty::~StringProperty(){
#ifdef _DEBUG_PROPERTIES
	cout << "~StringProperty(): start destruction!" << " : " << getName() << endl;
	cout << "~StringProperty(): end destruction!" << " : " << getName() << endl;
#endif
}

void StringProperty::init(){
#ifdef _DEBUG_PROPERTIES
	cout << "start void StringProperty::init(){" << endl;
#endif

	if (!initialized) {
		initialized = true;
	}

#ifdef _DEBUG_PROPERTIES
	cout << "end void StringProperty::init(){" << endl;
#endif
}

string StringProperty::toString(string v){
#ifdef _DEBUG_PROPERTIES
	cout << "string StringProperty::toString(String [" << v << "])" << endl;
#endif
	return v;
}

string StringProperty::toValue(string s){
#ifdef _DEBUG_PROPERTIES
	cout << "int StringProperty::toValue(string [" << s << "])" << endl;
#endif
	return s;
}

string StringProperty::toValue(char *c){
#ifdef _DEBUG_PROPERTIES
	cout << "int StringProperty::toValue(char *[" << c << "])" << endl;
#endif
	return string(c);
}

void StringProperty::setValue(char *c){
#ifdef _DEBUG_PROPERTIES
	cout << "void StringProperty::setValue(char *[" << c << "])" << " : " << getName() << endl;
#endif
	setValue(toValue(c));
}

string StringProperty::toString(){
#ifdef _DEBUG_PROPERTIES
	cout << "string StringProperty::toString() : " << getValue() << " : " << getName() << endl;
#endif
	return getValue();
}

StringProperty StringProperty::operator = (char* c){
	setValue(c);
	return *this;
}

StringProperty StringProperty::operator = (string s){
	setValue(s);
	return *this;
}
