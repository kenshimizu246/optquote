#include <string>
#include <sstream>
#include <vector>
#include <mutex>
#include <time.h>
#include <memory>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/named_scope.hpp>

#include "Dao.hpp"
#include "Leg.hpp"
#include "Session.hpp"
#include "SessionManager.hpp"
#include "ConversationManager.hpp"
#include "Conversation.hpp"



using namespace std;

namespace optquote {

ConversationManager::ConversationManager(){

}

ConversationManager::~ConversationManager(){

}

shared_ptr<Conversation> ConversationManager::getConversation(int conversationId){
	shared_ptr<Conversation> c = nullptr;
	auto found = conversations.find(conversationId);
	if(found != end(conversations)){
		c = found->second;
	} else {
		c = shared_ptr<Conversation>(new Conversation());
//		dao->load(conversationId, c);
	}
	return c;
}

void ConversationManager::add(shared_ptr<Conversation> c){
	conversations[c->id] = c;
}

void ConversationManager::sendConversationEvent(shared_ptr<Conversation> c){
	ConversationChangeEvent event{c};
	fireChangeListeners(event);
}

void ConversationManager::fireChangeListeners(ConversationChangeEvent event){
	BOOST_LOG_TRIVIAL(info) << "fireChangeListeners...";
	for(ConversationChangeListener* l: listeners){
		BOOST_LOG_TRIVIAL(info) << "fireChangeListeners... l";
		l->update(event);
	}
}

void ConversationManager::getMyConversations(vector<shared_ptr<Conversation>>& cc, string& userId){
	shared_ptr<Session> session = sessionManager->getSessionByUserID(userId);

	for(auto itr = conversations.begin(); itr != conversations.end(); ++itr){
		shared_ptr<Conversation> c = itr->second;
		if(session != nullptr && session->role == "trader"){
			cc.push_back(c);
		} else if(c->userId == userId || c->traderId == userId){
			cc.push_back(c);
		}
	}
}

void ConversationManager::checkTimeout(){
	time_t now = time(nullptr);

//	BOOST_LOG_TRIVIAL(info) << "ConversationManager:start timeout check...";
	for(auto itr = conversations.begin(); itr != conversations.end(); ++itr){
		shared_ptr<Conversation> c = itr->second;
//		BOOST_LOG_TRIVIAL(info) << "ConversationManager:check quote status[" << c->id << "][" << c->toString(c->status) << "]";
		if(c->status != Status::QUOTE){
			continue;
		}
		if(c->quoteExpire < now){
//			BOOST_LOG_TRIVIAL(info) << "ConversationManager:TIMEOUT:" << c->id;
		
			c->status = Status::TIMEOUT;
			c->quoteRemain = 0;	
		} else {
			c->quoteRemain = c->quoteExpire - now;	
		}
		sendConversationEvent(c);
	}
//	BOOST_LOG_TRIVIAL(info) << "ConversationManager:end timeout check...";
}

void ConversationManager::remove(shared_ptr<Conversation> c){
	lock_guard<mutex> lock(mtx);
	conversations.erase(c->id);
}

void ConversationManager::doHousekeeping(){
	time_t expire = time(nullptr);
	expire -= (60 * 60 * 24);

	lock_guard<mutex> lock(mtx);

	BOOST_LOG_TRIVIAL(info) << "ConversationManager:before clean:" << conversations.size();
	for(auto itr = conversations.begin(); itr != conversations.end();){
		shared_ptr<Conversation> c = itr->second;

		if(c->tradeDate < expire){
			BOOST_LOG_TRIVIAL(info) << "ConversationManager:should be clean:" << c->id;
			itr = conversations.erase(itr);
		}else{
			itr++;
		}
	}
	BOOST_LOG_TRIVIAL(info) << "ConversationManager:after clean:" << conversations.size();
}

}
