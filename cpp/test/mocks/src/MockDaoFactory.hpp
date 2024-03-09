#include <string>
#include <sstream>
#include <vector>
#include <mutex>

using namespace std;
using namespace optquote;

#include "../../../src/Dao.hpp"
#include "../../../src/DaoFactory.hpp"

#ifndef _mockdaofactory_h
#define _mockdaofactory_h

//class MockDaoFactory : public DaoFactory {
class MockDaoFactory : public DaoFactory {
	public:
		MockDaoFactory();
		~MockDaoFactory();

		shared_ptr<Dao> create();
};

#endif /* _mockdaofactory_h */
