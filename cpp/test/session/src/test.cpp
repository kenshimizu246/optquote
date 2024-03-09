#include <iostream>

using namespace std;

#include "../../../src/SessionManager.hpp"

int main() {
	cout << "test\n";
	SessionManager sessionManager;

	Session* sa = sessionManager.login(123,"ken", "pass", "admin");

	sessionManager.logout("123");

	Session* sb = sessionManager.getSessionBySessionID(sa->id.getValue());
	Session* sc = sessionManager.getSessionByUserID(sa->userId.getValue());


	cout << "ss:" << endl;
}

