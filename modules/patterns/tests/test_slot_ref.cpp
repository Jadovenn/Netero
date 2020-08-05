/**
* Netero sources under BSD-3-Clause
* see LICENSE.txt
*/

#include <memory>

#include <netero/slot.hpp>

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

    // test slot as ref
    netero::slot<int(int)>  slot(&Number::add, &integer);
    netero::slot<int(int)> &ref_slot = slot;
    ref_slot(10);

    // test slot as ptr
    netero::slot<int(int)> *ptr_slot = &slot;
    (*ptr_slot)(11);

    // test slot as shared ptr
    std::shared_ptr<netero::slot<int(int)>> shared_slot =
        std::make_shared<netero::slot<int(int)>>(&Number::add, &integer);
    (*shared_slot)(20);
    shared_slot.reset();

    // test with ref in signatures and non default ctor
    netero::slot<Number &(Number &)> ref_ndflt_ctor(inc);
    ref_ndflt_ctor(integer);

    EXPECT_EQ(integer.get(), 42);
}
