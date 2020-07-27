/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/type_id.hpp>

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
    netero::type_id a = netero::TypeID<BaseType>::getTypeID<TypeA>();
    netero::type_id b = netero::TypeID<BaseType>::getTypeID<TypeB>();
    netero::type_id c = netero::TypeID<BaseType>::getTypeID<TypeC>();

    EXPECT_EQ(a, netero::TypeID<BaseType>::getTypeID<TypeA>());
    EXPECT_EQ(b, netero::TypeID<BaseType>::getTypeID<TypeB>());
    EXPECT_EQ(c, netero::TypeID<BaseType>::getTypeID<TypeC>());
    EXPECT_TRUE(a != b && b != c & c != a);
}
