/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/ecs/component.hpp>
#include <netero/ecs/component_filter.hpp>

#include <gtest/gtest.h>

struct CompA: public netero::ecs::Component {
    int data;
};

struct CompB: public netero::ecs::Component {
    char data;
};

using GroupAB = netero::ecs::ComponentFilter<CompA, CompB>;
using GroupA = netero::ecs::ComponentFilter<CompA>;
using GroupB = netero::ecs::ComponentFilter<CompB>;
using GroupC = netero::ecs::ComponentFilter<>;

TEST(NeteroPatterns, component_filter)
{
    const netero::set<std::size_t> &filterAB = GroupAB::getFilter();
    const netero::set<std::size_t> &filterA = GroupA::getFilter();
    const netero::set<std::size_t> &filterB = GroupB::getFilter();
    const netero::set<std::size_t> &filterC = GroupC::getFilter();
    EXPECT_TRUE(filterA.isSubsetOf(filterAB));
    EXPECT_TRUE(filterB.isSubsetOf(filterAB));
    EXPECT_FALSE(filterA.isSubsetOf(filterB));
    EXPECT_FALSE(filterAB.isSubsetOf(filterA));
}
