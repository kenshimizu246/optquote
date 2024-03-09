#include <string>
#include <sstream>
#include <vector>
#include <mutex>

using namespace std;

#include "../../../src/Dao.hpp"

#ifndef _mockdao_h
#define _mockdao_h

class MockDao : public Dao{
	public:
		MockDao();
		~MockDao();

		int countCommit = 0;
		int countRollback = 0;
		int countClose = 0;

		void getSymbolList(vector<string>& symbols);
		void commit(){
			countCommit++;
		}
		void rollback(){
			countRollback++;
		}
		void close(){
			countClose++;
		}

};

#endif /* _mockdao_h */
