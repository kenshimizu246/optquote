#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <mutex>

#include "../../../src/WebSocketWriter.hpp"
#include "MockWebSocketWriter.hpp"

using namespace std;

MockWebSocketWriter::MockWebSocketWriter(string id) : id(id){
}

MockWebSocketWriter::~MockWebSocketWriter(){
}

int MockWebSocketWriter::write(string& ss){
	cout << "write[" << id << "]:" << ss << endl;
	return ss.length();
}



