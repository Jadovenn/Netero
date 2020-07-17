/**
* Netero sources under BSD-3-Clause
* see LICENSE.txt
*/

#include <cassert>
#include <functional>
#include <iostream>

#include <netero/signal.hpp>
#include <netero/slot.hpp>

#include <gtest/gtest.h>

class TestClass {
    public:
    int nbCall = 0;

    int add(int a, int b)
    {
        this->nbCall += 1;
        return a + b;
    }

    int mult(int a, int b)
    {
        this->nbCall += 1;
        return a * b;
    }
};

int add(int a, int b)
{
    return a + b;
}

TEST(NeteroPatterns, test_copy_operator)
{
    TestClass                     test;
    netero::signal<int(int, int)> signal;
    netero::slot<int(int, int)>   slot(&TestClass::add, &test);
    netero::slot<int(int, int)>   slot_bis;
    signal.connect(&slot);
    slot_bis = slot;
    signal(21, 21);
    EXPECT_EQ(test.nbCall, 2);
}

TEST(NeteroPatterns, test_copy_operator_with_signal_in_both_slot)
{
    TestClass                     test;
    netero::signal<int(int, int)> signal;
    netero::signal<int(int, int)> signal_bis;
    netero::slot<int(int, int)>   slot(&TestClass::add, &test);
    netero::slot<int(int, int)>   slot_bis;
    signal.connect(&slot);
    signal_bis.connect(&slot_bis);
    slot_bis = slot;
    signal(21, 21);
    signal_bis(21, 21);
    EXPECT_EQ(test.nbCall, 2);
}

TEST(NeteroPatterns, test_move_operator)
{
    TestClass                     test;
    netero::signal<int(int, int)> signal;
    netero::slot<int(int, int)>   slot(&TestClass::add, &test);
    netero::slot<int(int, int)>   slot_bis;
    signal.connect(&slot);
    signal(21, 21);
    slot_bis = std::move(slot);
    EXPECT_EQ(slot_bis.count_signal(), 1);
    EXPECT_EQ(slot.count_signal(), 0);
    EXPECT_EQ(slot_bis(21, 21), 42);
    EXPECT_EQ(test.nbCall, 2);
}

TEST(NeteroPatterns, test_move_operator_with_signal_in_both_slot)
{
    TestClass                     test;
    TestClass                     test_bis;
    netero::signal<int(int, int)> signal;
    netero::signal<int(int, int)> signal_bis;
    netero::slot<int(int, int)>   slot(&TestClass::mult, &test);
    netero::slot<int(int, int)>   slot_bis(&TestClass::add, &test_bis);
    signal.connect(&slot);
    signal_bis.connect(&slot_bis);
    slot_bis = std::move(slot);
    EXPECT_EQ(slot_bis.count_signal(), 1);
    EXPECT_EQ(slot.count_signal(), 0);
    EXPECT_EQ(slot_bis(21, 2), 42);
}

TEST(NeteroPatterns, test_copy_ctor)
{
    TestClass                   test;
    netero::slot<int(int, int)> slot(&TestClass::add, &test);
    netero::slot<int(int, int)> slot_copy(slot);
    EXPECT_EQ(slot(21, 21), 42);
    EXPECT_EQ(slot_copy(21, 21), 42);
    EXPECT_EQ(test.nbCall, 2);
}

TEST(NeteroPatterns, test_copy_ctor_with_signal)
{
    TestClass                     test;
    netero::signal<int(int, int)> signal;
    netero::slot<int(int, int)>   slot(&TestClass::add, &test);
    netero::slot<int(int, int)>*  slot_copy;
    signal.connect(&slot);
    slot_copy = new netero::slot<int(int, int)>(slot);
    EXPECT_EQ((*slot_copy)(21, 21), 42);
    EXPECT_EQ(test.nbCall, 1);
}

TEST(NeteroPatterns, test_functor_call)
{
    std::function<int(int, int)> functor(&add);
    netero::slot<int(int, int)>  slot(functor);
    netero::slot<int(int, int)>  default_slot;
    default_slot.set(functor);
    EXPECT_EQ(slot(21, 21), 42);
    EXPECT_EQ(default_slot(21, 21), 42);
}

TEST(NeteroPatterns, test_class_call)
{
    TestClass                   test;
    netero::slot<int(int, int)> slot(&TestClass::add, &test);
    netero::slot<int(int, int)> default_slot;
    default_slot.set(&TestClass::add, &test);
    EXPECT_EQ(slot(21, 21), 42);
    EXPECT_EQ(default_slot(21, 21), 42);
}

TEST(NeteroPatterns, test_function_call)
{
    netero::slot<int(int, int)> slot(&add);
    netero::slot<int(int, int)> default_slot;
    default_slot.set(&add);
    EXPECT_EQ(slot(21, 21), 42);
    EXPECT_EQ(default_slot(21, 21), 42);
}

TEST(NeteroPatterns, test_bool_operator)
{
    netero::slot<int(int, int)> slot;
    if (slot) {
        assert(false);
    }
    slot.set(&add);
    assert(slot);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
