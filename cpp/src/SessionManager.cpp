
#include <string>
#include <iostream>
#include <mutex>
#include <map>
#include <exception>
#include <stdexcept>
#include <time.h>
#include <uuid/uuid.h>

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


#include "Session.hpp"
#include "SessionManager.hpp"

namespace optquote {

SessionManager::SessionManager() {
}

SessionManager::~SessionManager() {
	//clean maps
}

shared_ptr<Session> SessionManager::login(string& userId, string& passwd, string& role){
	shared_ptr<Session> s;

	lock_guard<mutex> lock(mtx);

	auto iter = sessionByUserId.find(userId);
	if(iter != end(sessionByUserId)){
		s = iter->second;
		BOOST_LOG_TRIVIAL(info) << "login:already logged in:" << userId << ":" << s->id;
	}else if(sessions.size() <= maxSession){
		uuid_t u;
		char buff[37];
		uuid_generate(u);
		uuid_unparse(u, buff);
		string id(buff);

		BOOST_LOG_TRIVIAL(info) << "login:session id:" << id
					<< ", userId:" << userId
					<< ", role:" << role
					;

		s = shared_ptr<Session>(new Session(id, userId, userId, role, true));
		sessions[id] = s;
		sessionByUserId[userId] = s;
		BOOST_LOG_TRIVIAL(info) << "login:session created:" << userId << ":" << id;
	}
	BOOST_LOG_TRIVIAL(info) << "login:sessions[size:" << sessions.size() << "][max:" << sessions.max_size() << "]";

	return s;
}

void SessionManager::add(shared_ptr<Session> s){
	BOOST_LOG_TRIVIAL(info) << "SessionManager::add:" << s->id << ":" << s->userId;
	lock_guard<mutex> lock(mtx);
	sessions[s->id] = s;
	sessionByUserId[s->userId] = s;
}

void SessionManager::logout(shared_ptr<Session> s){
	if(s == nullptr){
		BOOST_LOG_TRIVIAL(error) << "logout:Session is null!";
	} else {
		BOOST_LOG_TRIVIAL(info) << "logout.sessionId:" << s->id;
		s->valid = false;
		sessions.erase(s->id);
		sessionByUserId.erase(s->userId);
		if(s->writer){
			s->writer.reset();
		}

		BOOST_LOG_TRIVIAL(info) << "logout:" << s->userId;;
//		try{
//			delete s;
//		} catch(bad_alloc &e) {
//			BOOST_LOG_TRIVIAL(info) << "logout:bad_alloc:" << e.what();
//		} catch(...) {
//			BOOST_LOG_TRIVIAL(info) << "logout:unknown exception!";
//		}
	}
}

void SessionManager::logout(string& sessionId){
	lock_guard<mutex> lock(mtx);
	auto iter = sessions.find(sessionId);
	if(iter != end(sessions)){
		logout(iter->second);
	}
}

shared_ptr<Session> SessionManager::getSessionByUserID(string userId){
	shared_ptr<Session> s;

	lock_guard<mutex> lock(mtx);
	auto iter = sessionByUserId.find(userId);
	if(iter != end(sessionByUserId)){
		s = iter->second;
	}
	return s;
}

shared_ptr<Session> SessionManager::getSessionBySessionID(string sessionid){
	shared_ptr<Session> s;

	lock_guard<mutex> lock(mtx);
	auto iter = sessions.find(sessionid);
	if(iter != end(sessions)){
		s = iter->second;
	}
	return s;
}

bool SessionManager::isValid(string sessionId){
	auto found = sessions.find(sessionId);
	return found != end(sessions);
}

int SessionManager::count(){
	return sessions.size();
}

void SessionManager::invalidate(time_t& t){
	for(auto itr = sessions.begin(); itr != sessions.end(); ++itr) {
		shared_ptr<Session> ss = itr->second;

		if(ss->last_accessed < t){
			ss->valid = false;
			BOOST_LOG_TRIVIAL(info) << "invalidate:" << ss->userId;
		}
	}
}

void SessionManager::clean(){
	vector<shared_ptr<Session>> invalid;
	for(auto itr = sessions.begin(); itr != sessions.end(); ++itr) {
		shared_ptr<Session> ss = itr->second;

		if(!ss->valid){
			invalid.push_back(ss);
		}
	}
	for(shared_ptr<Session> ss : invalid) {
		logout(ss->userId);
	}
}

void SessionManager::update(ConversationChangeEvent event){
	string c_client("client");
	string c_trader("trader");

	BOOST_LOG_TRIVIAL(info) << "SessionManager.update" ;
	shared_ptr<Conversation> c = event.getConversation();

	for(auto itr = sessions.begin(); itr != sessions.end(); ++itr) {
		shared_ptr<Session> ss = itr->second;
		BOOST_LOG_TRIVIAL(info) << "SessionManager.update::ss.role:" << ss->role
														<< ":ss.userId:" << ss->userId
														<< ":c.userId:" << c->userId
														<< ":sessionid:" << ss->id;
		if(!ss->valid){
			BOOST_LOG_TRIVIAL(info) << "SessionManager.update::invalid:";
			continue;
		}
		if(ss->role == c_trader
				|| (ss->role == c_client && ss->userId == c->userId)){
			if(ss->writer){
				BOOST_LOG_TRIVIAL(info) << "SessionManager.update::write is ok";
				string st;
				c->toJsonString(st, ss);
				ss->writer->write(st);
			}else{
				BOOST_LOG_TRIVIAL(info) << "SessionManager.update::write is not ok";
			}
		}
	}
	BOOST_LOG_TRIVIAL(info) << "SessionManager.update done";
}

void SessionManager::setMaxSession(int cnt){
	maxSession = cnt;
}

int SessionManager::getMaxSession(){
	return maxSession;
}

} //com.tamageta.optquote
