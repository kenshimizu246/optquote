#include <string>
#include <exception>
#include <stdexcept>
#include <memory>

#include "Action.hpp"
#include "../AppCtx.hpp"
#include "../Dao.hpp"
#include "../Session.hpp"

using namespace std;

#ifndef _getmyconversations_hpp
#define _getmyconversations_hpp

namespace optquote {

class GetMyConversations 
		: public Action {
	private:
		AppCtx *appCtx;
		shared_ptr<Session> session;

	public:
		GetMyConversations(AppCtx* appCtx, shared_ptr<Session> session);
		~GetMyConversations();
		void doAction(unique_ptr<Document> rq, string& ss);
};

}

#endif /* _getmyconversations_hpp */
