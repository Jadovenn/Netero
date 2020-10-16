/**
* Netero sources under BSD-3-Clause
* see LICENSE.txt
*/

#include <memory>

#include <Netero/Slot.hpp>

#include <gtest/gtest.h>

class Number {
    public:
    Number() = delete;
    explicit Number(const Number &) = delete;
    const Number &operator=(const Number &) = delete;

    explicit Number(int nb): number(nb) {}

    int add(int other)
    {
        number += other;
        return number;
    }

    [[nodiscard]] int get() const { return number; }

    private:
    int number;
};

// do some work with refs
Number &inc(Number &nb)
{
    nb.add(1);
    return nb;
}

TEST(NeteroPatterns, slot_ref)
{
    Number integer(0);

    // test Slot as ref
    Netero::Slot<int(int)>  slot(&Number::add, &integer);
    Netero::Slot<int(int)> &ref_slot = slot;
    ref_slot(10);

    // test Slot as ptr
    Netero::Slot<int(int)> *ptr_Slot = &slot;
    (*ptr_Slot)(11);

    // test Slot as shared ptr
    std::shared_ptr<Netero::Slot<int(int)>> shared_slot =
        std::make_shared<Netero::Slot<int(int)>>(&Number::add, &integer);
    (*shared_slot)(20);
    shared_slot.reset();

    // test with ref in signatures and non default ctor
    Netero::Slot<Number &(Number &)> ref_ndflt_ctor(inc);
    ref_ndflt_ctor(integer);

    EXPECT_EQ(integer.get(), 42);
}
