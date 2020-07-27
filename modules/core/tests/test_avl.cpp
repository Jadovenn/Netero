/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

// add debug info to netero, enable display method
#define NETERO_DEBUG

#include <iostream>
#include <memory>

#include <netero/avl.hpp>
#include <netero/logger.hpp>

#include <gtest/gtest.h>

TEST(NeteroCore, avl_general_usage)
{
    netero::avl<int> avl_tree;

    avl_tree.add(5);
    avl_tree.add(2);
    avl_tree.add(1);
    avl_tree.add(12);
    avl_tree.add(24);
    avl_tree.add(8);
    avl_tree.add(3);
    avl_tree.add(4);
    EXPECT_TRUE(avl_tree.search(1));
    EXPECT_FALSE(avl_tree.search(23));
    LOG_DEBUG << "Range base for-loop compatibility:" << std::endl;
    for (const auto &e : avl_tree) { // perform in-order traversal
        LOG_DEBUG << e << std::endl;
    }
    avl_tree.remove(1);
    assert(!avl_tree.search(1));
    avl_tree.remove(2);
    avl_tree.remove(12);
    avl_tree.display();
}

TEST(NeteroCore, avl_ctor_and_operator)
{
    netero::avl<int> tree1;
    tree1.add(1);
    tree1.add(2);
    tree1.add(3);
    tree1.add(4);
    netero::avl<int> tree2(tree1);
    EXPECT_TRUE(tree2.search(1));
    std::shared_ptr<netero::avl<int>> tree3 = std::make_shared<netero::avl<int>>();
    tree3->add(42);
    tree3->add(45);
    tree2 = *tree3;
    EXPECT_FALSE(tree2.search(1));
    EXPECT_TRUE(tree2.search(42));
    tree1 = std::move(tree2);
    EXPECT_FALSE(tree2.search(42));
    tree3.reset();
}
