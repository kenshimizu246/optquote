#include <string>
#include <exception>
#include <stdexcept>

#include "Action.hpp"
#include "../AppCtx.hpp"
#include "../Dao.hpp"

using namespace std;

namespace optquote {

class GetSymbolListAction 
		: public Action {
	private:
		AppCtx *appCtx;

	public:
		GetSymbolListAction(AppCtx* appCtx);
		~GetSymbolListAction();
		void doAction(unique_ptr<Document> rq, string& ss);
};

}


