/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

// add debug info to netero, enable display method
#define NETERO_DEBUG

#include <cassert>
#include <iostream>
#include <memory>

#include <netero/avl.hpp>

void basic_test()
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
    assert(avl_tree.search(1));
    assert(!avl_tree.search(23));
    std::cout << "Range base for-loop compatibility:" << std::endl;
    for (const auto &e : avl_tree) { // perform in-order traversal
        std::cout << e << std::endl;
    }
    avl_tree.remove(1);
    assert(!avl_tree.search(1));
    avl_tree.remove(2);
    avl_tree.remove(12);
    avl_tree.display();
}

void ctor_and_operator_test()
{
    netero::avl<int> tree1;
    tree1.add(1);
    tree1.add(2);
    tree1.add(3);
    tree1.add(4);
    netero::avl<int> tree2(tree1);
    assert(tree2.search(1));
    std::shared_ptr<netero::avl<int>> tree3 = std::make_shared<netero::avl<int>>();
    tree3->add(42);
    tree3->add(45);
    tree2 = *tree3;
    assert(!tree2.search(1));
    assert(tree2.search(42));
    tree1 = std::move(tree2);
    assert(!tree2.search(42));
    tree3.reset();
}

int main()
{
    basic_test();
    ctor_and_operator_test();
    return 0;
}
