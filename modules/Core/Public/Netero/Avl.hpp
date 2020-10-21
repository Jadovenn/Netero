/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file Avl.hpp
 * @brief Balanced binary search tree.
 */

#include <functional>
#include <list>
#include <memory>
#include <new>
#include <type_traits>

namespace Netero {
/**
 * @brief Adelson Velsky Landis (AVL) tree
 * @tparam T the type hold by the container
 */
template<class T,
         class Allocator = std::allocator<T>,
         typename = std::enable_if<std::is_copy_constructible<T>::value>>
class Avl {
    /**
     * @brief Structure representing a node in the tree,
     *        it hold the data provide by the client.
     */
    struct Node {
        template<typename... DataCtorArgs>
        explicit Node(Node *myParent, DataCtorArgs... args)
            : myBalance(0), myParent(myParent), myRhs(nullptr), myLhs(nullptr), myData(args...)
        {
        }

        Node(const Node &) = delete;
        Node(Node &&) = delete;
        Node &operator=(const Node &) = delete;
        Node &operator=(Node &&) = delete;

        /**
         * @brief compute the depth of a Node recursively by descending to all child of the Node
         * @return the depth
         */
        int GetDepth()
        {
            if (!myRhs && !myLhs)
                return 0;
            if (!myRhs)
                return myLhs->GetDepth() + 1;
            else if (!myLhs)
                return myRhs->GetDepth() + 1;
            else {
                int x = myLhs->GetDepth() + 1;
                int y = myRhs->GetDepth() + 1;
                return x > y ? x : y;
            }
        }

        /**
         * @brief compute the balance of a single Node (rhs - myLhs)
         * @details instead of going recursively to the root it only
         *          compute the Node, it does not balance the tree
         */
        void SingleBalance()
        {
            const int x = myRhs ? myRhs->GetDepth() : -1;
            const int y = myLhs ? myLhs->GetDepth() : -1;
            myBalance = x - y;
        }

        int   myBalance; /**< Current balance of the Node. */
        Node *myParent;  /**< Pointer to the parent Node. */
        Node *myRhs;     /**< Pointer to the right subtree root. */
        Node *myLhs;     /**< Pointer to left subtree root. */
        T     myData;    /**< Data pointer. */
    };

    /**
     * @brief compute the balance of a tree recursively
     * @details it start from the Node of the first call
     * and climb to the root of the tree, it does balance the tree
     * if necessary
     */
    void ComputeBalance(Node *item)
    {
        item->SingleBalance();
        if (item->myBalance == 2 || item->myBalance == -2)
            this->BalanceTree(item); // will call computeBalance again
        else if (item->myParent)
            ComputeBalance(item->myParent);
    }

    /**
     * @brief balance the tree depending the balance factor
     * @details case 1: (2)(1) the right subtree is heavy on right
     *          case 2: (-2)(-1) the left subtree is heavy on left
     *          case 3: (2)(-1) the right subtree as a left subtree heavy
     *          case 4: (-2)(1) the left subtree as a right subtree heavy
     */
    void BalanceTree(Node *item)
    {
        if (item->myBalance == 2) {            // case 1 or 3
            if (item->myRhs->myBalance == 1) { // case 1
                RotateLeft(item);
                ComputeBalance(item);
            }
            else if (item->myRhs->myBalance == -1) { // case 3
                RotateRight(item->myRhs);
                Node *subtree = RotateLeft(item);
                subtree->myLhs->SingleBalance();
                ComputeBalance(subtree->myRhs);
            }
        }
        else if (item->myBalance == -2) {       // case 2 or 4
            if (item->myLhs->myBalance == -1) { // case 2
                RotateRight(item);
                ComputeBalance(item);
            }
            else if (item->myLhs->myBalance == 1) { // case 4
                RotateLeft(item->myLhs);
                Node *subtree = RotateRight(item);
                subtree->myLhs->SingleBalance();
                ComputeBalance(subtree->myRhs);
            }
        }
    }

    /**
     * @brief Rotate the given tree to the right and return the new root of the subtree.
     * @param subtree is the root of a tree to rotate.
     * @return The new root of the subtree.
     */
    Node *RotateRight(Node *subtree)
    {
        Node *new_root = subtree->myLhs;
        new_root->myParent = subtree->myParent;
        subtree->myParent = new_root;
        Node *tmp_subRight = new_root->myRhs;
        new_root->myRhs = subtree;
        subtree->myLhs = tmp_subRight;
        if (tmp_subRight) // update subRight parent to subtree
            tmp_subRight->myParent = subtree;
        if (new_root->myParent) {
            if (new_root->myParent->myRhs == subtree)
                new_root->myParent->myRhs = new_root;
            else if (new_root->myParent->myLhs == subtree)
                new_root->myParent->myLhs = new_root;
        }
        else
            myRoot = new_root;
        return new_root; // no balance recomputing at this point
    }

    /**
     * @brief rotate the given tree to the left and return the new root of the subtree
     * @param subtree - root of a subtree
     * @return new root of the subtree
     */
    Node *RotateLeft(Node *subtree)
    {
        Node *new_root = subtree->myRhs;
        new_root->myParent = subtree->myParent;
        subtree->myParent = new_root;
        Node *tmp_subLeft = new_root->myLhs;
        new_root->myLhs = subtree;
        subtree->myRhs = tmp_subLeft;
        if (tmp_subLeft) // update subLeft parent to subtree
            tmp_subLeft->myParent = subtree;
        if (new_root->myParent) {
            if (new_root->myParent->myRhs == subtree)
                new_root->myParent->myRhs = new_root;
            else if (new_root->myParent->myLhs == subtree)
                new_root->myParent->myLhs = new_root;
        }
        else
            this->myRoot = new_root;
        return new_root; // no balance recomputing at this point
    }
    using NodeAllocator = typename Allocator::template rebind<Node>::other;

    public:
    /**
     * @brief iterator for the container to be compatible
     * with std function and for-range based loop
     * @details it build a list following a in-order traversal of the tree and iterate over it
     * @warning This is bullshit, need to rewrite a tree class for netero
     */
    class iterator {
        public:
        friend Avl;
        using difference_type = T;
        using value_type = T;
        using pointer = const T *;
        using reference = const T &;
        using iterator_category = std::forward_iterator_tag;

        /**
         * @brief build an iterator from a tree and start at the bottom left
         * @param tree - to iterate over
         */
        explicit iterator(const Avl &tree)
        {
            auto *current = tree.myRoot;
            if (!tree.myRoot) {
                myCurrent = nullptr;
                myNext = nullptr;
            }
            else {
                while (current->myLhs) {
                    current = current->myLhs;
                }
                myCurrent = current;
                myNext = GetNext();
            }
        }

        iterator(): myCurrent(nullptr), myNext(nullptr) {}

        iterator &operator++()
        {
            myCurrent = myNext;
            myNext = GetNext();
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp(myCurrent);
            myCurrent = myNext;
            myNext = GetNext();
            return tmp;
        }

        T &operator*() { return myCurrent->myData; }

        /**
         * @brief eql comparator to another iterator
         * @param other - other iterator
         * @return true if equal, false otherwise
         */
        bool operator==(const iterator &other) const
        {
            return myCurrent == other.myCurrent && myNext == other.myNext;
        }

        /**
         * @brief not eql comparator to another iterator
         * @param other - other iterator
         * @return true if not eql, false otherwise
         */
        bool operator!=(const iterator &other) const { return !(*this == other); }

        protected:
        explicit iterator(Node *aNode)
        {
            myCurrent = aNode;
            myNext = GetNext();
        }

        private:
        Node *GetNext()
        {
            if (!myCurrent) {
                return nullptr;
            }
            if (myCurrent->myLhs && myCurrent->myData < myCurrent->myLhs->myData) {
                auto *leftChild = myCurrent->myLhs;
                while (leftChild->myLhs) {
                    leftChild = leftChild->myLhs;
                }
                return leftChild;
            }
            else if (myCurrent->myRhs && myCurrent->myData < myCurrent->myRhs->myData) {
                auto *rightChild = myCurrent->myRhs;
                while (rightChild->myLhs) {
                    rightChild = rightChild->myLhs;
                }
                return rightChild;
            }
            auto *parent = myCurrent->myParent;
            while (parent) {
                if (myCurrent->myData < parent->myData) {
                    return parent;
                }
                if (parent->myRhs) {
                    if (myCurrent->myData < parent->myRhs->myData) {
                        return parent->myRhs;
                    }
                }
                parent = parent->myParent;
            }
            return parent;
        }

        Node *myCurrent;
        Node *myNext;
    };

    friend iterator;

    /**
     * @brief return an iterator to the beginning of the tree following in-order traversal
     * @return iterator
     */
    iterator begin() const { return iterator(*this); }

    /**
     * @brief return an iterator to the end of the tree following the in-order traversal
     * @return iterator
     */
    iterator end() const { return iterator(); }

    Avl(): myRoot(nullptr) {};

    explicit Avl(const Avl<T> &copy): myRoot(nullptr)
    {
        for (const auto &value : copy) {
            Insert(value);
        }
    }

    explicit Avl(Avl<T> &&move): myRoot(nullptr)
    {
        myRoot = move.myRoot;
        move.myRoot = nullptr;
    }

    Avl &operator=(const Avl<T> &copy)
    {
        if (this == &copy) {
            return *this;
        }
        this->DeleteTree(myRoot);
        myRoot = nullptr;
        for (const auto &value : copy) {
            Insert(value);
        }
        return *this;
    }

    Avl &operator=(Avl<T> &&move) noexcept
    {
        this->DeleteTree(myRoot);
        myRoot = move.myRoot;
        move.myRoot = nullptr;
        return *this;
    }

    bool operator==(const Avl<T> &other)
    {
        auto thisIt = begin();
        auto otherIt = other.begin();

        while (thisIt != end()) {
            if (otherIt == other.end()) {
                return false;
            }
            if (*thisIt != *otherIt) {
                return false;
            }
            ++thisIt;
            ++otherIt;
        }
        if (otherIt == other.end()) {
            return true;
        }
        return false;
    }

    bool operator!=(const Avl<T> &other) { return !(*this == other); }

    virtual ~Avl()
    {
        DeleteTree(myRoot);
        myRoot = nullptr;
    }

    /**
     * @brief Find if the given item exist in the tree
     * @param data - the item to look for
     * @return true if it is found or false otherwise
     */
    iterator Find(const T &aValue)
    {
        for (Node *idx = myRoot; idx;) {
            if (idx->myData == aValue)
                return iterator(idx);
            else if (idx->myData < aValue)
                idx = idx->myRhs;
            else
                idx = idx->myLhs;
        }
        return iterator();
    } // O(n) = n log(n)

    /**
     * @brief add a new Node to the tree by placing it without copy
     * @param data - the new item to add
     */
    template<typename... DataCtorArgs>
    iterator Emplace(DataCtorArgs... args)
    {
        Node *node = std::allocator_traits<NodeAllocator>::allocate(myNodeAllocator, 1);
        if (!node)
            return end();
        std::allocator_traits<NodeAllocator>::construct(myNodeAllocator, node, nullptr, args...);
        return InsertNode(node);
    }

    /**
     * @brief add a new Node to the tree by copy
     * @param data - the new item to add
     */
    iterator Insert(const T &aValue)
    {
        Node *node = std::allocator_traits<NodeAllocator>::allocate(myNodeAllocator, 1);
        if (!node)
            return end();
        std::allocator_traits<NodeAllocator>::construct(myNodeAllocator, node, nullptr, aValue);
        return InsertNode(node);
    }

    /**
     * @brief remove the given item from the tree.
     */
    void Remove(Avl::iterator it)
    {
        if (it == end()) {
            return;
        }
        RemoveNode(it.myCurrent);
    }

    void Remove(const T &aData) { Remove(Find(aData)); }

    private:
    iterator InsertNode(Node *aNode)
    {
        if (!aNode) // Special case, given pointer is null
            return iterator();
        aNode->myRhs = nullptr;
        aNode->myLhs = nullptr;
        if (!myRoot) { // Special case, three is empty add new data as root
            myRoot = aNode;
            ComputeBalance(myRoot);
            return iterator(myRoot);
        }
        { // Regular case, allocate and find the right place to add a leaf
            Node *parent = myRoot;
            Node *idx = myRoot;
            while (idx) {
                if (idx->myData == aNode->myData) { // Special case the node already exist
                    return iterator();
                }
                if (idx->myData < aNode->myData) {
                    parent = idx;
                    idx = idx->myRhs;
                }
                else {
                    parent = idx;
                    idx = idx->myLhs;
                }
            } // the node is found in parent add new_node as a leaf
            if (parent->myData < aNode->myData) { // Regular case, if new_data is bigger add right
                parent->myRhs = aNode;
            }
            else { // Regular case, if new_data is smaller add left
                parent->myLhs = aNode;
            }
            aNode->myParent = parent;
            aNode->myLhs = nullptr;
            aNode->myRhs = nullptr;
        } // end regular case context
        // Now we can balance stuff here
        ComputeBalance(aNode->myParent);
        return iterator(aNode);
    }

    void RemoveNode(Node *aNode)
    {
        Node *new_root;
        Node *parent = aNode->myParent;
        {                            // Regular case, delete the node and set correctly the new root
            if (parent == nullptr) { // Special case, idx is root
                new_root = DeleteNode(aNode);
                myRoot = new_root;
            }
            else if (parent->myLhs == aNode) {
                new_root = DeleteNode(aNode);
                parent->myLhs = new_root;
            }
            else {
                new_root = DeleteNode(aNode);
                parent->myRhs = new_root;
            }
        } // end regular case context
        // Now we can balance thing here
        // if a sub tree is returned, start balancing here
        if (new_root) {
            ComputeBalance(new_root);
        }
        // if no subtree is returned start balancing at parent, if parent is null do nothing
        else if (parent) {
            ComputeBalance(parent);
        }
    }

    /**
     * @brief Delete the entire tree.
     */
    void DeleteTree(Node *item)
    {
        if (!item)
            return;
        DeleteTree(item->myRhs);
        DeleteTree(item->myLhs);
        std::allocator_traits<NodeAllocator>::destroy(myNodeAllocator, item);
        std::allocator_traits<NodeAllocator>::deallocate(myNodeAllocator, item, 1);
        //delete item;
    }

    /**
     * @brief Delete a node and return the root of the new subtree
     * @param item is the node to be deleted.
     * @return The root of the tree.
     */
    Node *DeleteNode(Node *item)
    {
        Node *new_root = nullptr;
        if (!item->myLhs && !item->myRhs) { // Regular case is the node is leaf
            std::allocator_traits<NodeAllocator>::destroy(myNodeAllocator, item);
            std::allocator_traits<NodeAllocator>::deallocate(myNodeAllocator, item, 1);
            //delete item;
            return new_root;
        }
        if (item->myLhs && item->myRhs) { // Regular case, item is a node with two child
            Node *idx = item->myRhs;
            while (idx->myRhs) // Step 1, find the bigger elem in the right sub-tree
                idx = idx->myRhs;
            item->myData = std::move(idx->myData);
            Node *idx_parent = idx->myParent; // Step 4, remove the bigger elem initial node
            idx_parent->myRhs = DeleteNode(idx);
            if (idx_parent->myRhs) // Step 5, make sure the parent link is correct
                idx_parent->myRhs->myParent = idx_parent;
            return item; // Step 6, return the initial node with the data field updated
        }
        else if (item->myLhs) { // Regular case, item is a node with only lhs
            new_root = item->myLhs;
            new_root->myParent = item->myParent;
            std::allocator_traits<NodeAllocator>::destroy(myNodeAllocator, item);
            std::allocator_traits<NodeAllocator>::deallocate(myNodeAllocator, item, 1);
            //delete item;
            return new_root;
        }
        else if (item->myRhs) { // Regular case, item is node with only rhs
            new_root = item->myRhs;
            new_root->myParent = item->myParent;
            std::allocator_traits<NodeAllocator>::destroy(myNodeAllocator, item);
            std::allocator_traits<NodeAllocator>::deallocate(myNodeAllocator, item, 1);
            //delete item;
            return new_root;
        }
        return nullptr; // should never be reach
    }

    Node *        myRoot;          /**< The root node of the tree container. */
    NodeAllocator myNodeAllocator; /** The node allocator. It is based on the provide allocator. */
};

} // namespace Netero
