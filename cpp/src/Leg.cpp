#include <string>
#include <sstream>
#include <vector>
#include <mutex>
#include <ctime>

#include "Leg.hpp"

using namespace std;

namespace optquote {

Leg::Leg(){
}
Leg::Leg(int id, LS ls, CP cp, double quantity, double strike, time_t maturity)
			:id(id),ls(ls),cp(cp),premium(0),quantity(quantity),strike(strike),maturity(maturity)
				,volatility(0),delta(0),gamma(0),vega(0),theta(0),rho(0){
}
Leg::~Leg(){
}

}

