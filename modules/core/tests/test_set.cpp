/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <cassert>

#include <netero/set.hpp>

int main()
{
    netero::set<int> a { 1, 2, 3, 4, 5, 6, 7, 8 };
    netero::set<int> b { 6, 7, 8 };
    std::set<int>    c { 4, 5, 6 };
    netero::set<int> d(c);
    netero::set<int> e { 5, 10 };

    assert(!b.isSubsetOf(c));
    assert(b.isSubsetOf(a));
    assert(!e.isSubsetOf(a));
    assert(e.interWith(a));
    return 0;
}
