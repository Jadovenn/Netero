/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/Set.hpp>

#include <gtest/gtest.h>

TEST(NeteroCore, set_basic_usage)
{
    Netero::Set<int> a { 1, 2, 3, 4, 5, 6, 7, 8 };
    Netero::Set<int> b { 6, 7, 8 };
    std::set<int>    c { 4, 5, 6 };
    Netero::Set<int> d(c);
    Netero::Set<int> e { 5, 10 };

    EXPECT_FALSE(b.IsSubsetOf(c));
    EXPECT_TRUE(b.IsSubsetOf(a));
    EXPECT_FALSE(e.IsSubsetOf(a));
    EXPECT_TRUE(e.InterWith(a));
}
