#include <sstream>
#include <vector>
#include <time.h>

#include "Session.hpp"

using namespace std;

namespace optquote {

Session::Session(string id, string userId, string userName, string role, bool valid)
	:id(id),userId(userId),userName(userName),role(role),valid(valid){
	time(&start_time);
	time(&last_accessed);
}

Session::Session(string id, string userId, string userName, string role, bool valid, time_t start_time)
	:id(id),userId(userId),userName(userName),role(role),valid(valid),start_time(start_time){
	time(&last_accessed);
}

Session::~Session() {
}

void Session::update(){
	time(&last_accessed);
}

}

