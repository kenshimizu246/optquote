#include <string>
#include <exception>
#include <stdexcept>
#include <memory>

#include "Action.hpp"
#include "../AppCtx.hpp"
#include "../ConversationManager.hpp"
#include "../Dao.hpp"
#include "../Session.hpp"

using namespace std;

namespace optquote {

class ExecuteAction 
		: public Action {
	private:
		AppCtx *appCtx;
		shared_ptr<Session> session;

	public:
		ExecuteAction(AppCtx* appCtx, shared_ptr<Session> session);
		~ExecuteAction();
		void doAction(unique_ptr<Document> rq, string& ss);
};

}

