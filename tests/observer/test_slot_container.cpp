/**
* Netero sources under BSD-3-Clause
* see LICENCE.txt
*/

#include <cassert>
#include <functional>
#include <netero/observer/slot.hpp>
#include <netero/observer/signal.hpp>

class TestClass {
public:
	int		nbCall = 0;

	int 	add(int a, int b) {
		this->nbCall += 1;
		return a + b;
	}
};

int		add(int a, int b) {
	return a + b;
}

void test_copy_operator() {
	TestClass test;
	netero::signal<int(int, int)> signal;
	netero::slot<int(int, int)>	slot(&TestClass::add, &test);
	netero::slot<int(int, int)> slot_bis;
	signal.connect(&slot);
	slot_bis = slot;
	signal(1, 1);
	assert(test.nbCall == 2);
}

void	test_copy_ctor() {
	TestClass	test;
	netero::slot<int(int, int)>	slot(&TestClass::add, &test);
	netero::slot<int(int, int)>	slot_copy(slot);
	assert(slot(21, 21) == 42);
	assert(slot_copy(21, 21) == 42);
	assert(test.nbCall == 2);
}

void 	test_functor_call() {
	std::function<int(int, int)> functor(&add);
	netero::slot<int(int, int)>	slot(functor);
	netero::slot<int(int, int)>	default_slot;
	default_slot.set(functor);
	assert(slot(21, 21) == 42);
	assert(default_slot(21, 21) == 42);
}

void	test_class_call() {
	TestClass	test;
	netero::slot<int(int, int)>	slot(&TestClass::add, &test);
	netero::slot<int(int, int)>	default_slot;
	default_slot.set(&TestClass::add, &test);
	assert(slot(21, 21) == 42);
	assert(default_slot(21, 21) == 42);
}

void 	test_function_call() {
	netero::slot<int(int, int)>	slot(&add);
	netero::slot<int(int, int)>	default_slot;
	default_slot.set(&add);
	assert(slot(21, 21) == 42);
	assert(default_slot(21, 21) == 42);
}

void 	test_bool_operator() {
	netero::slot<int(int, int)>	slot;
	if (slot) { assert(false); }
	slot.set(&add);
	assert(slot);
}

int	main() {
	test_copy_operator();
	test_functor_call();
	test_function_call();
	test_class_call();
	test_copy_ctor();
	test_bool_operator();
	return 0;
}
