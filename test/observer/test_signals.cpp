/**
* Netero sources under BSD-3-Clause
* see LICENCE.txt
*/

#include <iostream>

#include "netero/observer/signals.hpp"

class	Subject {
public:
	explicit Subject(const std::string& name): _name(name), _age(0)
	{}

	int    operator++() {
		_age += 1;
		birthday(_name, _age);
		return _age;
	}

	netero::signals<void(std::string, int)>	birthday;
private:
	const std::string	&_name;
	int 				_age;
};

class	Observer {
public:
	Observer() : onBirthday(&Observer::notifyBirthday, this)
	{}

	void	notifyBirthday(std::string name, int age) {
		std::cout << "Happy birthday " << name << ", you have " << age << " now!" << std::endl;
	}

	netero::slot<void(std::string, int)>			onBirthday;
};


int 	basic_test() {
	Subject		bob("bob");
	Subject		marina("marina");
	Observer	notificationManager;
	bob.birthday.connect(&notificationManager.onBirthday);
	marina.birthday.connect(&notificationManager.onBirthday);
	++bob;
	++marina;
	return 0;
}

// corrupt subject by connecting corrupted observer
void 	corrupt_subject(Subject &subject) {
	Observer notificationManager;
	subject.birthday.connect(&notificationManager.onBirthday);
}

int 	delete_obs_before_call() {
	auto *subject = new Subject("bob");
	corrupt_subject(*subject);
	++*subject;
	return 0;
}

void 	corrupt_observer(Observer *obs) {
	Subject		bob("bob");
	bob.birthday.connect(&obs->onBirthday);
}

int		delete_subject_before_del_obs() {
	Observer	notificationManager;
	corrupt_observer(&notificationManager);
	return 0;
}

int main() {
	if (basic_test()) {
		return 1;
	}
	if (delete_obs_before_call()) {
		return 1;
	}
	if (delete_subject_before_del_obs()) {
		return 1;
	}
	return 0;
}
