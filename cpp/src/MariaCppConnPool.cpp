
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <vector>
#include <mutex>
#include <memory>

#include <mariacpp/lib.hpp>
#include <mariacpp/connection.hpp>
#include <mariacpp/exception.hpp>
#include <mariacpp/prepared_stmt.hpp>
#include <mariacpp/time.hpp>
#include <mariacpp/uri.hpp>

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

#include "MariaCppConnPool.hpp"

using namespace std;
using namespace MariaCpp;

namespace optquote {

ConnectionPool::ConnectionPool(string host, unsigned int port, string user, string passwd, string dbname, unsigned int min, unsigned int max)
: host(host), port(port), user(user), passwd(passwd), dbname(dbname), min(min), max(max), cntOpen(0), cntLeased(0), ping(nullptr)
{
	init();
}

ConnectionPool::~ConnectionPool(){
	BOOST_LOG_TRIVIAL(info) << "~ConnectionPool:conns_mutex lock";
	lock_guard<mutex> lock(conns_mutex);
	BOOST_LOG_TRIVIAL(info) << "~ConnectionPool:conns_mutex locked";

	if(ping != nullptr){
//		delete ping;
	}

	while(connections.size() > 0){
		Connection* conn = connections.back();
		connections.pop_back();
		conn->close();
		delete conn;
		cntOpen--;
	}
	BOOST_LOG_TRIVIAL(info) << "~ConnectionPool::cntOpen:" << cntOpen;
}

Connection* ConnectionPool::getConnection(){
	Connection *conn = nullptr;

	BOOST_LOG_TRIVIAL(info) << "getConnection:conns_mutex lock";
	lock_guard<mutex> lock(conns_mutex);
	BOOST_LOG_TRIVIAL(info) << "getConnection:conns_mutex locked";

	if(cntLeased < max){
		BOOST_LOG_TRIVIAL(info) << "getConnection::no conn";
		for(int i = 0; i < connections.size(); i++){
			BOOST_LOG_TRIVIAL(info) << "getConnection::for:" << i;
			conn = connections.back();
			BOOST_LOG_TRIVIAL(info) << "getConnection::conn:" << conn;
			connections.pop_back();

			try{
				BOOST_LOG_TRIVIAL(info) << "getConnection::ping";
				conn->ping(); // should fix, it makes coredump.
			}catch(...){
				BOOST_LOG_TRIVIAL(info) << "getConnection::exception";
				delete conn;
				conn = nullptr;
			}
		}
		if(conn == nullptr){
			BOOST_LOG_TRIVIAL(info) << "getConnection::create";
			conn = createConnection();
			cntOpen++;
			BOOST_LOG_TRIVIAL(info) << "getConnection::cntOpen:" << cntOpen;
		}
		cntLeased++;
		BOOST_LOG_TRIVIAL(info) << "getConnection::cntLeased:" << cntLeased;
	} else {
		BOOST_LOG_TRIVIAL(info) << "getConnection::cntLeased:max:" << cntLeased;
	}
	return conn;
}

void ConnectionPool::releaseConnection(Connection* conn){
	BOOST_LOG_TRIVIAL(info) << "releaseConnection::cntLeased:start:" << cntLeased;
	BOOST_LOG_TRIVIAL(info) << "releaseConnection:conns_mutex lock";
	lock_guard<mutex> lock(conns_mutex);
	BOOST_LOG_TRIVIAL(info) << "releaseConnection:conns_mutex locked";
	connections.push_back(conn);
	cntLeased--;
	BOOST_LOG_TRIVIAL(info) << "releaseConnection::cntLeased:end:" << cntLeased;
}

Connection* ConnectionPool::createConnection(){
	Connection *conn;
	conn = new Connection();

	stringstream suri;
	suri << "tcp://" << host << ":" << port << "/" << dbname;

	BOOST_LOG_TRIVIAL(info) << "ConnectionPool::createConnection:" << suri << ":" << user << ":" << passwd;

	conn->connect(Uri(suri.str().c_str()),user.c_str(),passwd.c_str(),0);

	conn->autocommit(false);

	return conn;
}

void ConnectionPool::init(){
	BOOST_LOG_TRIVIAL(info) << "initConnection::cntOpen:start:" << cntOpen;
	BOOST_LOG_TRIVIAL(info) << "initConnection:conns_mutex lock";
	lock_guard<mutex> lock(conns_mutex);
	BOOST_LOG_TRIVIAL(info) << "ConnectionPool::init::locked";
	for(unsigned int i = 0; i < min; i++){
		Connection *conn;
		conn = createConnection();
		connections.push_back(conn);
		BOOST_LOG_TRIVIAL(info) << "ConnectionPool::cntOpen:" << cntOpen;
		cntOpen++;
	}
	BOOST_LOG_TRIVIAL(info) << "ConnectionPool::create ping check thread...";
	ping = new thread([this](){
		BOOST_LOG_TRIVIAL(info) << "ConnectionPool::start ping check...";
		try{
			BOOST_LOG_TRIVIAL(info) << "ConnectionPool::start lock";
			lock_guard<mutex> lock(conns_mutex);
			BOOST_LOG_TRIVIAL(info) << "ConnectionPool::conns_mutex locked";
			for(int i = connections.size() - 1; i != 0; --i){
				try{
					Connection *conn = connections[i];
					BOOST_LOG_TRIVIAL(info) << "check()::ping:" << i;
					conn->ping(); // should fix, it makes coredump.
				}catch(...){
					BOOST_LOG_TRIVIAL(info) << "check()::exception:" << i;
					connections.erase(connections.begin() + i);
				}
			}
		}catch(...){
			BOOST_LOG_TRIVIAL(info) << "check()::exception";
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000*60*60));
	});

	BOOST_LOG_TRIVIAL(info) << "initConnection::cntOpen:end:" << cntOpen;
}

}

