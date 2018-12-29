/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "../../Headers/netero/netero.hpp"

struct BaseType {};
struct TypeA : public BaseType {};
struct TypeB : public BaseType {};
struct TypeC : public BaseType {};

int		main() {
	netero::type_id a = netero::TypeID<BaseType>::getTypeID<TypeA>();
	netero::type_id b = netero::TypeID<BaseType>::getTypeID<TypeB>();
	netero::type_id c = netero::TypeID<BaseType>::getTypeID<TypeC>();

	if (a != netero::TypeID<BaseType>::getTypeID<TypeA>())
		return 1;
	if (b != netero::TypeID<BaseType>::getTypeID<TypeB>())
		return 2;
	if (c != netero::TypeID<BaseType>::getTypeID<TypeC>())
		return 3;
	if (a == b || b == c || c == a)
		return 4;
	return 0;
}
