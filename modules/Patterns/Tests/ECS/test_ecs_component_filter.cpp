/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <Netero/ECS/Component.hpp>
#include <Netero/ECS/ComponentFilter.hpp>

#include <gtest/gtest.h>

struct CompA: public Netero::ECS::Component {
    int data;
};

struct CompB: public Netero::ECS::Component {
    char data;
};

using GroupAB = Netero::ECS::ComponentFilter<CompA, CompB>;
using GroupA = Netero::ECS::ComponentFilter<CompA>;
using GroupB = Netero::ECS::ComponentFilter<CompB>;
using GroupC = Netero::ECS::ComponentFilter<>;

TEST(NeteroPatterns, component_filter)
{
    const Netero::Set<std::size_t> &filterAB = GroupAB::GetFilter();
    const Netero::Set<std::size_t> &filterA = GroupA::GetFilter();
    const Netero::Set<std::size_t> &filterB = GroupB::GetFilter();
    const Netero::Set<std::size_t> &filterC = GroupC::GetFilter();
    EXPECT_TRUE(filterA.IsSubsetOf(filterAB));
    EXPECT_TRUE(filterB.IsSubsetOf(filterAB));
    EXPECT_FALSE(filterA.IsSubsetOf(filterB));
    EXPECT_FALSE(filterAB.IsSubsetOf(filterA));
}
