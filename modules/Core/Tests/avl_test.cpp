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
    EXPECT_TRUE(avl_tree.Empty());

    avl_tree.Insert(5);
    avl_tree.Insert(2);
    avl_tree.Insert(1);
    avl_tree.Insert(12);
    avl_tree.Insert(24);
    avl_tree.Insert(8);
    avl_tree.Insert(3);
    avl_tree.Insert(4);
    avl_tree.Insert(9);
    avl_tree.Insert(10);
    avl_tree.Insert(11);

    EXPECT_NE(avl_tree.Find(1), avl_tree.end());
    EXPECT_EQ(avl_tree.Find(23), avl_tree.end());
    avl_tree.Remove(1);
    avl_tree.Remove(2);
    avl_tree.Remove(12);
    EXPECT_EQ(avl_tree.Find(1), avl_tree.end());
    avl_tree.Insert(5);
    avl_tree.Insert(6);
    avl_tree.Insert(7);

    avl_tree.Remove(5);
    avl_tree.Remove(24);
    avl_tree.Remove(8);
    avl_tree.Remove(3);
    avl_tree.Remove(4);
    avl_tree.Remove(9);
    avl_tree.Remove(10);
    avl_tree.Remove(11);
}

TEST(NeteroCore, Avl_insert_emplace_test)
{
    Netero::Avl<int> tree;
    tree.Insert(4);
    auto it = tree.Insert(5);
    EXPECT_EQ(*it, 5);
    it = tree.Insert(5);
    EXPECT_EQ(it, tree.end());
    it = tree.Emplace(6);
    EXPECT_EQ(*it, 6);
}

TEST(NeteroCore, Avl_remove_test)
{
    Netero::Avl<int> tree;
    tree.Insert(4);
    auto it = tree.Find(4);
    EXPECT_EQ(*it, 4);
    tree.Remove(it);
    it = tree.Find(4);
    EXPECT_EQ(it, tree.end());
    EXPECT_NO_FATAL_FAILURE(tree.Remove(it));
}

TEST(NeteroCore, Avl_iterator_test)
{
    Netero::Avl<int> linearInsertTree;

    for (unsigned idx = 100; idx > 0; idx--) {
        linearInsertTree.Insert(idx);
    }
    for (const auto& number : linearInsertTree) {
        LOG_INFO << number << std::endl;
    }

    Netero::Avl<int> tree;

    tree.Insert(5);
    tree.Insert(2);
    tree.Insert(1);
    tree.Insert(12);
    tree.Insert(24);
    tree.Insert(8);
    tree.Insert(3);
    tree.Insert(4);
    tree.Insert(9);
    tree.Insert(10);
    tree.Insert(11);

    for (const auto& number : tree) {
        LOG_INFO << number << std::endl;
    }

    auto it = tree.begin();
    EXPECT_EQ(*it, 1);
    auto postIt = it++;
    EXPECT_EQ(*postIt, 1);
    EXPECT_EQ(*it, 2);

    Netero::Avl<int> emptyTree;
    auto             emptyIt = emptyTree.begin();
    EXPECT_EQ(emptyIt, emptyTree.end());
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

TEST(NeteroCore, Avl_copy_move_ctor)
{
    Netero::Avl<int> tree;
    tree.Insert(1);
    tree.Insert(2);
    Netero::Avl<int> copy(tree);
    EXPECT_NE(copy.Find(2), copy.end());
    Netero::Avl<int> move(std::move(copy));
    EXPECT_NE(move.Find(1), move.end());
    EXPECT_EQ(copy.Find(2), copy.end());
    copy = std::move(move);
    EXPECT_EQ(move.Find(1), move.end());
    EXPECT_NE(copy.Find(2), copy.end());
}

TEST(NeteroCore, Avl_comparison_op)
{
    Netero::Avl<int> a;
    a.Insert(-5);
    a.Insert(1);
    a.Insert(4);
    Netero::Avl<int> b;
    b.Insert(1);
    b.Insert(4);
    b.Insert(-5);
    Netero::Avl<int> c;
    c.Insert(1);
    c.Insert(4);
    c.Insert(100);
    Netero::Avl<int> d;
    d.Insert(1);
    d.Insert(4);
    d.Insert(-5);
    d.Insert(100);
    Netero::Avl<int> e;
    e.Insert(-5);
    e.Insert(1);

    EXPECT_TRUE(a == b);
    EXPECT_TRUE(a != c);
    EXPECT_FALSE(b == c);
    EXPECT_FALSE(a == d);
    EXPECT_FALSE(a == e);
}
