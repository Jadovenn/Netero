/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/TypeId.hpp>

#include <gtest/gtest.h>

struct BaseType {
};
struct TypeA: public BaseType {
};
struct TypeB: public BaseType {
};
struct TypeC: public BaseType {
};

TEST(NeteroCore, type_id_basic_usage)
{
    Netero::type_id a = Netero::TypeID<BaseType>::GetTypeID<TypeA>();
    Netero::type_id b = Netero::TypeID<BaseType>::GetTypeID<TypeB>();
    Netero::type_id c = Netero::TypeID<BaseType>::GetTypeID<TypeC>();

    EXPECT_EQ(a, Netero::TypeID<BaseType>::GetTypeID<TypeA>());
    EXPECT_EQ(b, Netero::TypeID<BaseType>::GetTypeID<TypeB>());
    EXPECT_EQ(c, Netero::TypeID<BaseType>::GetTypeID<TypeC>());
    EXPECT_TRUE(a != b && b != c & c != a);
}
