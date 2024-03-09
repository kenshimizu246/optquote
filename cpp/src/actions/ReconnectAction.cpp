#include <iostream>
#include <sstream>
#include <pthread.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

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

#include "ReconnectAction.hpp"
#include "../AppCtx.hpp"

using namespace rapidjson;
using namespace std;

namespace optquote {

ReconnectAction::ReconnectAction(AppCtx *appCtx, shared_ptr<Session> session) : appCtx(appCtx), session(session) {
}

ReconnectAction::~ReconnectAction(){
	BOOST_LOG_TRIVIAL(info) << "ReconnectAction::~ReconnectAction()";
}

void ReconnectAction::doAction(unique_ptr<Document> rq, string& ss){
	Document res;
	Document::AllocatorType& alloc = res.GetAllocator();
	Value hdr(kObjectType);
	Value bdy(kObjectType);

  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);

	BOOST_LOG_TRIVIAL(info) << "ReconnectAction::doAction()...start";

	if(!(*rq)["body"].HasMember("userId")){
		throw invalid_argument("userId is mandatory!");
	}

	if(!(*rq)["body"].HasMember("role")){
		throw invalid_argument("role is mandatory!");
	}

	if(strcmp(session->userId.c_str(), (*rq)["body"]["userId"].GetString()) != 0){
		throw invalid_argument("userId is not mutch! " + session->userId);
	}

	if(strcmp(session->role.c_str(), (*rq)["body"]["role"].GetString()) != 0){
		throw invalid_argument("role is not mutch! " + session->role);
	}

	// start boot strap.
/*
	thread tp([&dao,&session](){
		vector<shared_ptr<Price>> pp;
		dao->getPriceList(pp);
		BOOST_LOG_TRIVIAL(info) << "price size : " << pp.size();
		for(shared_ptr<Price> p : pp){
			string ss;
			p->toJsonString(ss);
			BOOST_LOG_TRIVIAL(info) << "price : " << ss;
			session->writer->write(ss);
		}
	});
*/
	
	vector<shared_ptr<Conversation>> cc;
	appCtx->getConversationManager()->getMyConversations(cc, session->userId);
	BOOST_LOG_TRIVIAL(info) << "conversation size : " << cc.size();
	for(shared_ptr<Conversation> c: cc){
		string ss;
		c->toJsonString(ss, session);
		BOOST_LOG_TRIVIAL(info) << "conversation : " << ss;
		if(session->writer){
			session->writer->write(ss);
		}
	}

//	tp.join();
// 	tc.join();


	hdr.AddMember("type", "reconnect_response", alloc);
	hdr.AddMember("sessionId", StringRef(session->id.c_str()), alloc);

	bdy.AddMember("message", "Hello", alloc);

  res.SetObject()
    .AddMember("header", hdr, alloc)
    .AddMember("body", bdy, alloc);

  res.Accept(writer);

  ss.append(buffer.GetString(), buffer.GetLength());

	BOOST_LOG_TRIVIAL(info) << "ReconnectAction::doAction()...end";
}

}


