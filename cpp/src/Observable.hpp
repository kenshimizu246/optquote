
#include <string>
#include <exception>
#include <stdexcept>

using namespace std;

namespace optquote {

emum Type{
	ADD, DELETE, MODIFIED
}

class ChangeEvent {
	private:
		Observable source;
		Type type
}

class Observable {
	private:
		string id;

	public:
		string getId();
};

}
