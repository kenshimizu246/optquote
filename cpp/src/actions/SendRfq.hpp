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

class SendRfq 
		: public Action {
	private:
		AppCtx *appCtx;
		shared_ptr<Session> session;

	public:
		SendRfq(AppCtx* appCtx, shared_ptr<Session> session);
		~SendRfq();
		void doAction(unique_ptr<Document> rq, string& ss);
};

}
