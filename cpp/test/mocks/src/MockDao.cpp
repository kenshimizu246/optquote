#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <mutex>
#include <memory>

#include "../../../src/Dao.hpp"
#include "MockDao.hpp"

using namespace std;
using namespace optquote;

MockDao::MockDao(){
}

MockDao::~MockDao(){
}

void MockDao::getSymbolList(vector<string>& symbols){
	symbols.push_back("TEST1");
	symbols.push_back("TEST2");
	symbols.push_back("TEST3");
	symbols.push_back("TEST4");
	symbols.push_back("TEST5");
}

void MockDao::sendRfq(shared_ptr<Conversation> c){

}

void MockDao::takeOwnership(shared_ptr<Conversation> c){

}

void MockDao::quote(shared_ptr<Conversation> c){

}

void MockDao::timeoutQuote(shared_ptr<Conversation> c){

}

void MockDao::counter(shared_ptr<Conversation> c){

}

void MockDao::order(shared_ptr<Conversation> c){

}

void MockDao::execute(shared_ptr<Conversation> c){

}

void MockDao::chat(shared_ptr<Conversation> c, string& userId, string& message){

}

void MockDao::insert(shared_ptr<Conversation> c){

}

void MockDao::update(shared_ptr<Conversation> c){

}

void MockDao::load(int id, shared_ptr<Conversation> c){

}

void MockDao::load(vector<int>& ids, string& start, string& end){

}

void MockDao::load(vector<shared_ptr<Conversation>>& conversations, vector<int>& ids){

}

void MockDao::load(ConversationManager& cm){

}

void MockDao::load(SessionManager& sm){

}


void MockDao::getRfqList(vector<shared_ptr<Conversation>> rfqs){

}

void MockDao::getPriceList(vector<shared_ptr<Price>>& prices){

}


unsigned int MockDao::addLoginHistory(string& userId, string& role, string& sessionId){
	return 0;
}

void MockDao::addAccessHistory(shared_ptr<Session> session){

}



