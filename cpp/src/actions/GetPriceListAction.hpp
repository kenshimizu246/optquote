#include <string>
#include <exception>
#include <stdexcept>

#include "Action.hpp"
#include "../AppCtx.hpp"
#include "../Dao.hpp"

using namespace std;

namespace optquote {

class GetPriceListAction 
		: public Action {
	private:
		AppCtx *appCtx;

	public:
		GetPriceListAction(AppCtx* appCtx);
		~GetPriceListAction();
		void doAction(unique_ptr<Document> rq, string& ss);
};

}

