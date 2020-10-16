/**
* Netero sources under BSD-3-Clause
* see LICENSE.txt
*/

#include <functional>
#include <iostream>

#include <Netero/Signal.hpp>
#include <Netero/Slot.hpp>

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
    Netero::Signal<int(int, int)> signal;
    Netero::Slot<int(int, int)>   Slot(&TestClass::add, &test);
    Netero::Slot<int(int, int)>   Slot_bis;
    signal.Connect(&Slot);
    Slot_bis = Slot;
    signal(21, 21);
    EXPECT_EQ(test.nbCall, 2);
}

TEST(NeteroPatterns, test_copy_operator_with_signal_in_both_Slot)
{
    TestClass                     test;
    Netero::Signal<int(int, int)> signal;
    Netero::Signal<int(int, int)> signalBis;
    Netero::Slot<int(int, int)>   Slot(&TestClass::add, &test);
    Netero::Slot<int(int, int)>   Slot_bis;
    signal.Connect(&Slot);
    signalBis.Connect(&Slot_bis);
    Slot_bis = Slot;
    signal(21, 21);
    signalBis(21, 21);
    EXPECT_EQ(test.nbCall, 2);
}

TEST(NeteroPatterns, test_move_operator)
{
    TestClass                     test;
    Netero::Signal<int(int, int)> signal;
    Netero::Slot<int(int, int)>   slot(&TestClass::add, &test);
    Netero::Slot<int(int, int)>   slotBis;
    signal.Connect(&slot);
    signal(21, 21);
    slotBis = std::move(slot);
    EXPECT_EQ(slotBis.SignalCount(), 1);
    EXPECT_EQ(slot.SignalCount(), 0);
    EXPECT_EQ(slotBis(21, 21), 42);
    EXPECT_EQ(test.nbCall, 2);
}

TEST(NeteroPatterns, test_move_operator_with_signal_in_both_Slot)
{
    TestClass                     test;
    TestClass                     test_bis;
    Netero::Signal<int(int, int)> signal;
    Netero::Signal<int(int, int)> signalBis;
    Netero::Slot<int(int, int)>   slot(&TestClass::mult, &test);
    Netero::Slot<int(int, int)>   slotBis(&TestClass::add, &test_bis);
    signal.Connect(&slot);
    signalBis.Connect(&slotBis);
    slotBis = std::move(slot);
    EXPECT_EQ(slotBis.SignalCount(), 1);
    EXPECT_EQ(slot.SignalCount(), 0);
    EXPECT_EQ(slotBis(21, 2), 42);
}

TEST(NeteroPatterns, test_copy_ctor)
{
    TestClass                   test;
    Netero::Slot<int(int, int)> slot(&TestClass::add, &test);
    Netero::Slot<int(int, int)> slotCopy(slot);
    EXPECT_EQ(slot(21, 21), 42);
    EXPECT_EQ(slotCopy(21, 21), 42);
    EXPECT_EQ(test.nbCall, 2);
}

TEST(NeteroPatterns, test_copy_ctor_with_signal)
{
    TestClass                     test;
    Netero::Signal<int(int, int)> signal;
    Netero::Slot<int(int, int)>   slot(&TestClass::add, &test);
    Netero::Slot<int(int, int)>*  slotCopy;
    signal.Connect(&slot);
    slotCopy = new Netero::Slot<int(int, int)>(slot);
    EXPECT_EQ((*slotCopy)(21, 21), 42);
    EXPECT_EQ(test.nbCall, 1);
}

TEST(NeteroPatterns, test_functor_call)
{
    std::function<int(int, int)> functor(&add);
    Netero::Slot<int(int, int)>  slot(functor);
    Netero::Slot<int(int, int)>  defaultSlot;
    defaultSlot.Set(functor);
    EXPECT_EQ(slot(21, 21), 42);
    EXPECT_EQ(defaultSlot(21, 21), 42);
}

TEST(NeteroPatterns, test_class_call)
{
    TestClass                   test;
    Netero::Slot<int(int, int)> slot(&TestClass::add, &test);
    Netero::Slot<int(int, int)> defaultSlot;
    defaultSlot.Set(&TestClass::add, &test);
    EXPECT_EQ(slot(21, 21), 42);
    EXPECT_EQ(defaultSlot(21, 21), 42);
}

TEST(NeteroPatterns, test_function_call)
{
    Netero::Slot<int(int, int)> slot(&add);
    Netero::Slot<int(int, int)> default_slot;
    default_slot.Set(&add);
    EXPECT_EQ(slot(21, 21), 42);
    EXPECT_EQ(default_slot(21, 21), 42);
}

TEST(NeteroPatterns, test_bool_operator)
{
    Netero::Slot<int(int, int)> Slot;
    if (Slot) {
        assert(false);
    }
    Slot.Set(&add);
    assert(Slot);
}
