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
    EXPECT_NE(avl_tree.Find(1), avl_tree.end());
    EXPECT_EQ(avl_tree.Find(23), avl_tree.end());
    avl_tree.Remove(1);
    avl_tree.Remove(2);
    avl_tree.Remove(12);
    EXPECT_EQ(avl_tree.Find(1), avl_tree.end());
}

TEST(NeteroCore, Avl_iterator_test)
{
    Netero::Avl<int> tree;

    for (unsigned idx = 100; idx > 0; idx--) {
        tree.Insert(idx);
    }
    for (const auto& number : tree) {
        LOG_INFO << number << std::endl;
    }
}

TEST(NeteroCore, Avl_copy_operator_test)
{
    Netero::Avl<int> tree;
    tree.Insert(1);
    tree.Insert(2);
    tree = tree;
    Netero::Avl<int> copyTree;
    copyTree = tree;
    EXPECT_NE(copyTree.Find(2), tree.end());
}

TEST(NeteroCore, avl_ctor_and_operator)
{
    Netero::Avl<int> tree1;
    tree1.Insert(1);
    tree1.Insert(2);
    tree1.Insert(3);
    tree1.Insert(4);
}
