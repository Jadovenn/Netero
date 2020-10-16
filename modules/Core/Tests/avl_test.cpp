/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <memory>

#include <Netero/Avl.hpp>
#include <Netero/Logger.hpp>

#include <gtest/gtest.h>

TEST(NeteroCore, avl_general_usage)
{
    Netero::Avl<int> avl_tree;

    avl_tree.Insert(5);
    avl_tree.Insert(2);
    avl_tree.Insert(1);
    avl_tree.Insert(12);
    avl_tree.Insert(24);
    avl_tree.Insert(8);
    avl_tree.Insert(3);
    avl_tree.Insert(4);
    EXPECT_TRUE(avl_tree.Find(1));
    EXPECT_FALSE(avl_tree.Find(23));
    LOG_DEBUG << "Range base for-loop compatibility:" << std::endl;
    for (const auto &e : avl_tree) { // perform in-order traversal
        LOG_DEBUG << e << std::endl;
    }
    avl_tree.Remove(1);
    assert(!avl_tree.Find(1));
    avl_tree.Remove(2);
    avl_tree.Remove(12);
}

TEST(NeteroCore, avl_ctor_and_operator)
{
    Netero::Avl<int> tree1;
    tree1.Insert(1);
    tree1.Insert(2);
    tree1.Insert(3);
    tree1.Insert(4);
    Netero::Avl<int> tree2(tree1);
    EXPECT_TRUE(tree2.Find(1));
    std::shared_ptr<Netero::Avl<int>> tree3 = std::make_shared<Netero::Avl<int>>();
    tree3->Insert(42);
    tree3->Insert(45);
    tree2 = *tree3;
    EXPECT_FALSE(tree2.Find(1));
    EXPECT_TRUE(tree2.Find(42));
    tree1 = std::move(tree2);
    EXPECT_FALSE(tree2.Find(42));
    tree3.reset();
}
