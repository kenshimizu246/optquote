#include <string>
#include <sstream>
#include <vector>
#include <mutex>
#include <ctime>

using namespace std;

#ifndef _leg_h
#define _leg_h

namespace optquote {

enum LS {
	Long=0,
	Short=1
};

enum CP {
	Call=0,
	Put=1
};

class Leg {
	public:
		Leg();
		Leg(int id, LS ls, CP cp, double quantity, double strike, time_t maturity);
		~Leg();

		int id;

		LS ls;
		CP cp;

		// option
		double premium;
		double quantity;
		double strike;
		time_t maturity;
		double volatility;

		// greeks
		double delta;
		double gamma;
		double vega;
		double theta;
		double rho;

	private:
		mutex lock;
};

}
#endif /* _leg_h */
