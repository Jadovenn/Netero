/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <cassert>

#include <netero/type_id.hpp>

struct BaseType {
};
struct TypeA: public BaseType {
};
struct TypeB: public BaseType {
};
struct TypeC: public BaseType {
};

int main()
{
    netero::type_id a = netero::TypeID<BaseType>::getTypeID<TypeA>();
    netero::type_id b = netero::TypeID<BaseType>::getTypeID<TypeB>();
    netero::type_id c = netero::TypeID<BaseType>::getTypeID<TypeC>();

    assert(a == netero::TypeID<BaseType>::getTypeID<TypeA>());
    assert(b == netero::TypeID<BaseType>::getTypeID<TypeB>());
    assert(c == netero::TypeID<BaseType>::getTypeID<TypeC>());
    assert(a != b && b != c & c != a);
    return 0;
}
