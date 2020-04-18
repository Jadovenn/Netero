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

	int		mult(int a, int b) {
		this->nbCall += 1;
		return a * b;
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
	signal(21, 21);
	assert(test.nbCall == 2);
}

void test_copy_operator_with_signal_in_both_slot() {
	TestClass test;
	TestClass test_bis;
	netero::signal<int(int, int)> signal;
	netero::signal<int(int, int)> signal_bis;
	netero::slot<int(int, int)>	slot(&TestClass::mult, &test);
	netero::slot<int(int, int)> slot_bis(&TestClass::add, &test_bis);
	signal.connect(&slot);
	signal_bis.connect(&slot_bis);
	slot_bis = slot;
	signal(21, 21);
	signal_bis(21, 21);
	assert(test.nbCall == 2);
}

void test_move_operator()
{
	TestClass test;
	netero::signal<int(int, int)> signal;
	netero::slot<int(int, int)>	slot(&TestClass::add, &test);
	netero::slot<int(int, int)> slot_bis;
	signal.connect(&slot);
	signal(21, 21);
	slot_bis = std::move(slot);
	assert(slot_bis.count_signal() == 1);
	assert(slot.count_signal() == 0);
	assert(slot_bis(21, 21) == 42);
	assert(test.nbCall == 2);
}

void test_move_operator_with_signal_in_both_slot()
{
	TestClass test;
	TestClass test_bis;
	netero::signal<int(int, int)> signal;
	netero::signal<int(int, int)> signal_bis;
	netero::slot<int(int, int)>	slot(&TestClass::mult, &test);
	netero::slot<int(int, int)> slot_bis(&TestClass::add, &test_bis);
	signal.connect(&slot);
	signal_bis.connect(&slot_bis);

	slot_bis = std::move(slot);
	assert(slot_bis.count_signal() == 1);
	assert(slot.count_signal() == 0);
	assert(slot_bis(21, 2) == 42);
}

void	test_copy_ctor() {
	TestClass	test;
	netero::slot<int(int, int)>	slot(&TestClass::add, &test);
	netero::slot<int(int, int)>	slot_copy(slot);
	assert(slot(21, 21) == 42);
	assert(slot_copy(21, 21) == 42);
	assert(test.nbCall == 2);
}


void	test_copy_ctor_with_signal() {
	TestClass	test;
	netero::signal<int(int, int)> signal;
	netero::slot<int(int, int)>	slot(&TestClass::add, &test);
	netero::slot<int(int, int)>	*slot_copy;
	signal.connect(&slot);
	slot_copy = new netero::slot<int(int, int)>(slot);
	assert((*slot_copy)(21, 21) == 42);
	assert(test.nbCall == 1);
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
	test_copy_operator_with_signal_in_both_slot();
	test_move_operator();
	test_move_operator_with_signal_in_both_slot();
	test_functor_call();
	test_function_call();
	test_class_call();
	test_copy_ctor();
	test_copy_ctor_with_signal();
	test_bool_operator();
	return 0;
}
