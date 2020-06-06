/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

// add debug info to netero, enable display method
#define NETERO_DEBUG

#include <iostream>
#include <memory>
#include "netero/avl.hpp"

int basic_test() {
    netero::avl<int>        avl_tree;

    avl_tree.add(5);
    avl_tree.add(2);
    avl_tree.add(1);
    avl_tree.add(12);
    avl_tree.add(24);
    avl_tree.add(8);
    avl_tree.add(3);
    avl_tree.add(4);
    if (!avl_tree.search(1))
        return 1;
    if (avl_tree.search(23))
        return 1;
    std::cout << "Range base for-loop compatibility:" << std::endl;
    for (const auto &e: avl_tree) { // perform in-order traversal
        std::cout << e << std::endl;
    }
    avl_tree.remove(1);
    if (avl_tree.search(1))
        return 1;
    avl_tree.remove(2);
    avl_tree.remove(12);
    avl_tree.display();
    return 0;
}

int    ctor_and_operator_test() {
    netero::avl<int>         tree1;
    tree1.add(1);tree1.add(2);tree1.add(3);tree1.add(4);
    netero::avl<int>          tree2(tree1);
    if (!tree2.search(1))
        return 1;
    std::shared_ptr<netero::avl<int>>   tree3 = std::make_shared<netero::avl<int>>();
    tree3->add(42);tree3->add(45);
    tree2 = *tree3;
    if (tree2.search(1))
        return 1;
    if (!tree2.search(42))
        return 1;
    tree1 = std::move(tree2);
    if (tree2.search(42))
        return 1;
    tree3.reset();
    return 0;
}

int     main() {
    if (basic_test() != 0)
        return 1;
    if (ctor_and_operator_test() != 0)
        return 1;
    return 0;
}
