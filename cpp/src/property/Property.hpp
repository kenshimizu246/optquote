
#include <string>
#include <vector>
#include <mutex>
#include <algorithm>

using namespace std;

#ifndef _property_hpp
#define _property_hpp

enum PROP_STATUS{
	EMPTY,
	INIT,
	MOD
};

class ChangeEvent {
	public:
		ChangeEvent(PROP_STATUS status, string name, int changeId) : status(status), name(name), changeId(changeId) {};
		PROP_STATUS getStatus();
		
	private:
		PROP_STATUS status;
		string name;
		int changeId;
};

class ChangeListener {
	public:
		ChangeListener(){};
		virtual void update(ChangeEvent event);
};

//void ChangeListener::update(ChangeEvent event){
//	cout << "originalupdate" << endl;
//}

template <typename V>
class Property {
	public:
		Property() : status(EMPTY) {
			cout << "Property()" << endl;
		};

		Property(string pname) : status(EMPTY), name(pname) {
			cout << "Property(string [" << pname << "])" << endl;
		};

		Property(string pname, V initValue) : status(EMPTY), name(pname) {
			cout << "Property(string [" << pname << "], V [" << initValue << "])" << endl;
			setValue(initValue);
		};

		~Property() {
			cout << "~Property()" << endl;
		};

		Property(const Property<V>& p) {
			cout << "Property(Property& [" << "])" << endl;

			status = EMPTY;

			setName(p.getName());

			if(!p.isEmpty()){
				cout << "!empty" << endl;
				p.copyHistory(this);
				setValue(p.getValue());
			}

			listenersEnabled = p.isChangeListenerEnabled();
			p.copyChangeListener(this);
		};

		V getValue() const;
		void setValue(V value);
		virtual string toString(){
			cout << "Property virtual string toString()" << endl;
			return string();
		}
		void setName(string name);
		string getName() const;

		void addChangeListener(ChangeListener *listener);
		void removeChangeListener(ChangeListener *listener);
		void copyChangeListener(Property<V> *p) const;
		void copyHistory(Property<V> *p) const;
		void stopChangeListener();
		void startChangeListener();
		bool isChangeListenerEnabled() const;
		void clearChangeFlag();

		void setStatus(PROP_STATUS stat);
		PROP_STATUS getStatus() const;

		bool isEmpty() const {
			return status == EMPTY;
		}

		bool isDirty() const {
			return status == MOD;
		}

	private:
		mutex mtx;
		V value;
		PROP_STATUS status;
		string name;
		bool listenersEnabled = true;
		vector<ChangeListener*> listeners;
		vector<V> history;

		void fireChangeListeners(ChangeEvent event);
};

template <typename V>
void Property<V>::setName(string name){
	cout << "void Property<V>::setName(string [" << name << "])" << endl;
	this->name = name;
}

template <typename V>
string Property<V>::getName() const {
	cout << "string Property<V>::getName() : " << name << endl;
	return name;
}

template <typename V>
void Property<V>::setStatus(PROP_STATUS stat){
	cout << "void Property<V>::setStatus(PROP_STATUS [" << status << "]) " << endl;
	status = stat;
}

template <typename V>
PROP_STATUS Property<V>::getStatus() const {
	cout << "PROP_STATUS Property<V>::getStatus() : " << status << endl;
	return status;
}

template <typename V>
V Property<V>::getValue() const {
	cout << "V Property<V>::getValue() : " << value << endl;
	return value;
}

template <typename V>
void Property<V>::setValue(V v){
	cout << "void Property<V>::setValue(V v) : "
			<< "v[" << v << "] "
			<< "name[" << name << "] "
			<< "status[" << status << "] ";
	if( status == INIT || status == MOD ){
		cout	<< "value[" << value << "] ";
	}
	cout	<< endl;


	if (status == INIT || status == MOD){
		history.push_back(value);
		status = MOD;
		value = v;
	} else {
		status = INIT;
		value = v;
	}
	fireChangeListeners(ChangeEvent(status, getName(), history.size() - 1));
}

template <typename V>
void Property<V>::addChangeListener(ChangeListener *listener){
	cout << "void Property<V>::addChangeListener(ChangeListener *listener)" << endl;
	mtx.lock();
	listeners.push_back(listener);
	mtx.unlock();
}

template <typename V>
void Property<V>::removeChangeListener(ChangeListener *listener){
	cout << "void Property<V>::removeChangeListener(ChangeListener *listener)" << endl;
	mtx.lock();
	auto position = std::find(listeners.begin(), listeners.end(), listener);
	if(position != listeners.end()){
		listeners.erase(position);
	}
	mtx.unlock();
}

template <typename V>
void Property<V>::copyChangeListener(Property<V> *p) const {
	cout << "void Property<V>::copyChangeListener(Property<V>& p){" << endl;

	for(auto itr = listeners.begin(); itr != listeners.end(); ++itr) {
		p->addChangeListener(*itr);
	}
}

template <typename V>
void Property<V>::fireChangeListeners(ChangeEvent event){
	cout << "void Property<V>::fireChangeListeners(ChangeEvent event)" << endl;
	if(!listenersEnabled) {
		cout << "void Property<V>::fireChangeListeners(ChangeEvent event):listenersEnabled false" << endl;
		return;
	}
	mtx.lock();
	for(auto itr = listeners.begin(); itr != listeners.end(); ++itr) {
		(*itr)->update(event);
	}
	mtx.unlock();
}

template <typename V>
void Property<V>::copyHistory(Property<V> *p) const {
	cout << "void Property<V>::copyHistory(Property<V>& p){" << endl;

	for(auto itr = history.begin(); itr != history.end(); ++itr) {
		p->setValue(*itr);
	}
}

template <typename V>
void Property<V>::stopChangeListener(){
		listenersEnabled = false;
}

template <typename V>
void Property<V>::startChangeListener(){
		listenersEnabled = true;
}

template <typename V>
bool Property<V>::isChangeListenerEnabled() const {
		return listenersEnabled;
}

#endif /* _property_hpp */



