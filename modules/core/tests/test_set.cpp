/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/set.hpp>

#include <gtest/gtest.h>

TEST(NeteroCore, set_basic_usage)
{
    netero::set<int> a { 1, 2, 3, 4, 5, 6, 7, 8 };
    netero::set<int> b { 6, 7, 8 };
    std::set<int>    c { 4, 5, 6 };
    netero::set<int> d(c);
    netero::set<int> e { 5, 10 };

    EXPECT_FALSE(b.isSubsetOf(c));
    EXPECT_TRUE(b.isSubsetOf(a));
    EXPECT_FALSE(e.isSubsetOf(a));
    EXPECT_TRUE(e.interWith(a));
}
