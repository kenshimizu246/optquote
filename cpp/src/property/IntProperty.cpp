#include <string>
#include <sstream>
#include <vector>
#include <ql/quantlib.hpp>

#include "Property.hpp"
#include "IntProperty.hpp"

using namespace std;
using namespace QuantLib;

bool IntProperty::initialized = false;

IntProperty::~IntProperty(){
#ifdef _DEBUG_PROPERTIES
	cout << "~IntProperty(): start destruction!" << " : " << getName() << endl;
	cout << "~IntProperty(): end destruction!" << " : " << getName() << endl;
#endif
}

void IntProperty::init(){
#ifdef _DEBUG_PROPERTIES
	cout << "start void IntProperty::init(){" << endl;
#endif

	if (!initialized) {
		initialized = true;
	}

#ifdef _DEBUG_PROPERTIES
	cout << "end void IntProperty::init(){" << endl;
#endif
}

string IntProperty::toString(int v){
#ifdef _DEBUG_PROPERTIES
	cout << "string IntProperty::toString(Int [" << v << "])" << endl;
#endif
	return to_string(v);
}

int IntProperty::toValue(string s){
#ifdef _DEBUG_PROPERTIES
	cout << "int IntProperty::toValue(string [" << s << "])" << endl;
#endif
	return atoi(s.c_str());
}

int IntProperty::toValue(char *c){
#ifdef _DEBUG_PROPERTIES
	cout << "int IntProperty::toValue(char *[" << c << "])" << endl;
#endif
	return atoi(c);
}

void IntProperty::setValue(string s){
#ifdef _DEBUG_PROPERTIES
	cout << "void IntProperty::setValue(string [" << s << "])" << " : " << getName() << endl;
#endif
	setValue(toValue(s));
}

void IntProperty::setValue(char *c){
#ifdef _DEBUG_PROPERTIES
	cout << "void IntProperty::setValue(char *[" << c << "])" << " : " << getName() << endl;
#endif
	setValue(toValue(c));
}

string IntProperty::toString(){
#ifdef _DEBUG_PROPERTIES
	cout << "string IntProperty::toString() : " << getValue() << " : " << getName() << endl;
#endif
	return toString(getValue());
}

IntProperty IntProperty::operator = (int i){
	setValue(i);
	return *this;
}

IntProperty IntProperty::operator = (char* c){
	setValue(c);
	return *this;
}

IntProperty IntProperty::operator = (string s){
	setValue(s);
	return *this;
}




