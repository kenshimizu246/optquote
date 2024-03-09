
#include <string>
#include <exception>
#include <stdexcept>

using namespace std;

#ifndef _utils_h
#define _utils_h

namespace optquote {

class Utils {
	private:
		Utils();
		~Utils();

	public:
		static time_t convertToTimeT(const char *date);
};

}

#endif /* _utils_h */

