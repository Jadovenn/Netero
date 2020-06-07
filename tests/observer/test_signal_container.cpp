/**
* Netero sources under BSD-3-Clause
* see LICENSE.txt
*/

#include <cassert>
#include <iostream>
#include <functional>
#include <netero/observer/signal.hpp>

class	Subject {
public:
	explicit Subject(const std::string& name): _name(name), _age(0)
	{}

	int    operator++() {
		_age += 1;
		birthday(_name, _age);
		return _age;
	}

	netero::signal<void(const std::string&, int)>	birthday;
private:
	const std::string	&_name;
	int 				_age;
};

class	Observer {
public:
	Observer() : onBirthday(&Observer::notifyBirthday, this)
	{}

	void	notifyBirthday(const std::string& name, int age) {
		std::cout << "Observer(" << this << ") ";
		std::cout << "Happy birthday "<< name;
		std::cout << ", you have " << age << " now!" << std::endl;
	}

	netero::slot<void(const std::string&, int)>			onBirthday;
};


void 	basic_test() {
	Subject    bob("bob");
	Subject    marina("marina");
	Observer   notificationManager;
	bob.birthday.connect(&notificationManager.onBirthday);
	marina.birthday.connect(&notificationManager.onBirthday);
	++bob;
	++marina;
}

// corrupt subject by connecting corrupted observer
void 	corrupt_subject(Subject &subject) {
	Observer notificationManager;
	subject.birthday.connect(&notificationManager.onBirthday);
}

void 	delete_obs_before_call() {
	auto *subject = new Subject("bob");
	corrupt_subject(*subject);
	++*subject;
}

void 	corrupt_observer(Observer *obs) {
	Subject    bob("bob");
	bob.birthday.connect(&obs->onBirthday);
}

void	delete_subject_before_del_obs() {
	Observer	notificationManager;
	corrupt_observer(&notificationManager);
}

void    signal_connect_disconnect() {
    netero::signal<int(const int&, const int&)> signal;
    netero::slot<int(const int&, const int&)> slot1;
    netero::slot<int(const int&, const int&)> slot2;
    signal.connect(&slot1);
    signal.connect(&slot2);
    assert(signal.size() == 2);
    signal.disconnect(&slot2);
    assert(signal.size() == 1);
    signal.reset();
    assert(signal.size() == 0);

}

int main() {
    basic_test();
    delete_obs_before_call();
    delete_subject_before_del_obs();
    signal_connect_disconnect();
	return 0;
}
