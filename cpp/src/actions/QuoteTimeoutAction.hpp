#include <string>
#include <exception>
#include <stdexcept>

#include "Action.hpp"
#include "../AppCtx.hpp"
#include "../ConversationManager.hpp"
#include "../Dao.hpp"
#include "../Session.hpp"

using namespace std;

namespace optquote {

class QuoteTimeoutAction 
		: public Action {
	private:
		AppCtx *appCtx;
		shared_ptr<Session> session;

	public:
		QuoteTimeoutAction(AppCtx* appCtx, shared_ptr<Session> session);
		~QuoteTimeoutAction();
		void doAction(unique_ptr<Document> rq, string& ss);
};

}

