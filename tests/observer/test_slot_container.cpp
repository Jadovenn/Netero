/**
* Netero sources under BSD-3-Clause
* see LICENCE.txt
*/

#include <cassert>
#include <functional>
#include <netero/observer/slot.hpp>

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

void	test_copy_ctor() {
	TestClass	test;
	netero::slot<int(int, int)>	slot(&TestClass::add, &test);
	netero::slot<int(int, int)>	slot_copy(slot);
	if (slot(21, 21) != 42) { assert(false); }
	if (slot_copy(21, 21) != 42) { assert(false); }
	if (test.nbCall != 2) { assert(false); }
}

void 	test_functor_call() {
	std::function<int(int, int)> functor(&add);
	netero::slot<int(int, int)>	slot(functor);
	netero::slot<int(int, int)>	default_slot;
	default_slot.set(functor);
	if (slot(21, 21) != 42) { assert(false); }
	if (default_slot(21, 21) != 42) { assert(false); }
}

void	test_class_call() {
	TestClass	test;
	netero::slot<int(int, int)>	slot(&TestClass::add, &test);
	netero::slot<int(int, int)>	default_slot;
	default_slot.set(&TestClass::add, &test);
	if (slot(21, 21) != 42) { assert(false); }
	if (default_slot(21, 21) != 42) { assert(false); }
}

void 	test_function_call() {
	netero::slot<int(int, int)>	slot(&add);
	netero::slot<int(int, int)>	default_slot;
	default_slot.set(&add);
	if (slot(21, 21) != 42) { assert(false); }
	if (default_slot(21, 21) != 42) { assert(false); }
}

void 	test_bool_operator() {
	netero::slot<int(int, int)>	slot;
	if (slot) { assert(false); }
	slot.set(&add);
	if (!slot) { assert(false); }
}

int	main() {
	test_functor_call();
	test_function_call();
	test_class_call();
	test_copy_ctor();
	test_bool_operator();
	return 0;
}
