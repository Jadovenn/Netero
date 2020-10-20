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
     * @brief structure representing a node in the tree,
     * it hold the data provide by the client
     */
    struct node {
        explicit node(T *data, node *parent = nullptr)
            : balance(0), parent(parent), rhs(nullptr), lhs(nullptr), data(data)
        {
        }

        node(const node &) = delete;
        node(node &&) = delete;
        node &operator=(const node &) = delete;
        node &operator=(node &&) = delete;

        ~node()
        {
            //delete data;
        }

        /**
         * @brief compute the depth of a node recursively
         *         by descending to all child of the node
         * @return the depth
         */
        int getDepth()
        {
            if (!rhs && !lhs)
                return 0;
            if (!rhs)
                return lhs->getDepth() + 1;
            else if (!lhs)
                return rhs->getDepth() + 1;
            else {
                int x = lhs->getDepth() + 1;
                int y = rhs->getDepth() + 1;
                return x > y ? x : y;
            }
        }

        /**
         * @brief compute the balance of a single node (rhs - lhs)
         * @details instead of going recursively to the root
         * it only compute the node, it does not balance the tree
         */
        void singleBalance()
        {
            const int x = rhs ? rhs->getDepth() : -1;
            const int y = lhs ? lhs->getDepth() : -1;
            balance = x - y;
        }

        int   balance; /**< Current balance of the node. */
        node *parent;  /**< Pointer to the parent node. */
        node *rhs;     /**< Pointer to the right subtree root. */
        node *lhs;     /**< Pointer to left subtree root. */
        T *   data;    /**< Data pointer. */
    };

    /**
     * @brief compute the balance of a tree recursively
     * @details it start from the node of the first call
     * and climb to the root of the tree, it does balance the tree
     * if necessary
     */
    void computeBalance(node *item)
    {
        item->singleBalance();
        if (item->balance == 2 || item->balance == -2)
            this->balanceTree(item); // will call computeBalance again
        else if (item->parent)
            computeBalance(item->parent);
    }

    /**
     * @brief balance the tree depending the balance factor
     * @details case 1: (2)(1) the right subtree is heavy on right
     *          case 2: (-2)(-1) the left subtree is heavy on left
     *          case 3: (2)(-1) the right subtree as a left subtree heavy
     *          case 4: (-2)(1) the left subtree as a right subtree heavy
     */
    void balanceTree(node *item)
    {
        if (item->balance == 2) {          // case 1 or 3
            if (item->rhs->balance == 1) { // case 1
                rotateLeft(item);
                computeBalance(item);
            }
            else if (item->rhs->balance == -1) { // case 3
                rotateRight(item->rhs);
                node *subtree = rotateLeft(item);
                subtree->lhs->singleBalance();
                computeBalance(subtree->rhs);
            }
        }
        else if (item->balance == -2) {     // case 2 or 4
            if (item->lhs->balance == -1) { // case 2
                rotateRight(item);
                computeBalance(item);
            }
            else if (item->lhs->balance == 1) { // case 4
                rotateLeft(item->lhs);
                node *subtree = rotateRight(item);
                subtree->lhs->singleBalance();
                computeBalance(subtree->rhs);
            }
        }
    }

    /**
     * @brief Rotate the given tree to the right and return the new root of the subtree.
     * @param subtree is the root of a tree to rotate.
     * @return The new root of the subtree.
     */
    node *rotateRight(node *subtree)
    {
        node *new_root = subtree->lhs;
        new_root->parent = subtree->parent;
        subtree->parent = new_root;
        node *tmp_subRight = new_root->rhs;
        new_root->rhs = subtree;
        subtree->lhs = tmp_subRight;
        if (tmp_subRight) // update subRight parent to subtree
            tmp_subRight->parent = subtree;
        if (new_root->parent) {
            if (new_root->parent->rhs == subtree)
                new_root->parent->rhs = new_root;
            else if (new_root->parent->lhs == subtree)
                new_root->parent->lhs = new_root;
        }
        else
            this->root = new_root;
        return new_root; // no balance recomputing at this point
    }

    /**
     * @brief rotate the given tree to the left and return the new root of the subtree
     * @param subtree - root of a subtree
     * @return new root of the subtree
     */
    node *rotateLeft(node *subtree)
    {
        node *new_root = subtree->rhs;
        new_root->parent = subtree->parent;
        subtree->parent = new_root;
        node *tmp_subLeft = new_root->lhs;
        new_root->lhs = subtree;
        subtree->rhs = tmp_subLeft;
        if (tmp_subLeft) // update subLeft parent to subtree
            tmp_subLeft->parent = subtree;
        if (new_root->parent) {
            if (new_root->parent->rhs == subtree)
                new_root->parent->rhs = new_root;
            else if (new_root->parent->lhs == subtree)
                new_root->parent->lhs = new_root;
        }
        else
            this->root = new_root;
        return new_root; // no balance recomputing at this point
    }
    using NodeAllocator = typename Allocator::template rebind<node>::other;

    public:
    /**
     * @brief iterator for the container to be compatible
     * with std function and for-range based loop
     * @details it build a list following a in-order traversal of the tree and iterate over it
     * @warning This is bullshit, need to rewrite a tree class for netero
     */
    class iterator {
        public:
        using difference_type = T;
        using value_type = T;
        using pointer = const T *;
        using reference = const T &;
        using iterator_category = std::forward_iterator_tag;

        /**
         * @brief build an iterator from a tree and start at the bottom left
         * @param tree - to iterate over
         */
        explicit iterator(Avl &tree)
        {
            auto *current = tree.root;
            while (current->lhs) {
                current = current->lhs;
            }
            myPrev = nullptr;
            myCurrent = current;
            myNext = GetNext();
        }

        iterator(): myCurrent(nullptr), myNext(nullptr) {}

        /**
         * @brief increment the iterator to the next value
         * @return
         */
        iterator &operator++()
        {
            myPrev = myCurrent;
            myCurrent = myNext;
            myNext = GetNext();
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp(myCurrent);
            myPrev = myCurrent;
            myCurrent = myNext;
            myNext = GetNext();
            return tmp;
        }

        /**
         * @brief return the index value
         * @return
         */
        const T &operator*() const { return *myCurrent->data; }

        /**
         * @brief eql comparator to another iterator
         * @param other - other iterator
         * @return true if equal, false otherwise
         */
        bool operator==(iterator &other) const
        {
            return myCurrent == other.myCurrent && myNext == other.myNext;
        }

        /**
         * @brief not eql comparator to another iterator
         * @param other - other iterator
         * @return true if not eql, false otherwise
         */
        bool operator!=(iterator &other) const { return !(*this == other); }

        protected:
        explicit iterator(node *aNode)
        {
            myCurrent = aNode;
            myPrev = aNode->parent;
            myNext = GetNext();
        }

        private:
        node *GetNext()
        {
            if (!myCurrent) {
                return nullptr;
            }
            if (myCurrent->lhs && *myCurrent->data < *myCurrent->lhs->data) {
                auto *lhs = myCurrent->lhs;
                while (lhs->lhs) {
                    lhs = lhs->lhs;
                }
                return lhs;
            }
            else if (myCurrent->rhs && *myCurrent->data < *myCurrent->rhs->data) {
                auto *lhs = myCurrent->rhs;
                while (lhs->lhs) {
                    lhs = lhs->lhs;
                }
                return lhs;
            }
            auto *parent = myCurrent->parent;
            while (parent) {
                if (*myCurrent->data < *parent->data) {
                    return parent;
                }
                if (parent->rhs) {
                    if (*myCurrent->data < *parent->rhs->data) {
                        return parent->rhs;
                    }
                }
                parent = parent->parent;
            }
            return parent;
        }

        node *myCurrent;
        node *myNext;
        node *myPrev;
    };
    friend iterator;
    /**
     * @brief return an iterator to the beginning of the tree following in-order traversal
     * @return iterator
     */
    iterator begin() { return iterator(*this); }

    /**
     * @brief return an iterator to the end of the tree following the in-order traversal
     * @return iterator
     */
    iterator end() { return iterator(); }

    // default ctor
    Avl(): root(nullptr) {};

    // copy ctor, not efficient
    explicit Avl(const Avl<T> &copy): root(nullptr)
    {
        copy.InOrder([&](const auto &value) { this->Insert(value); });
    }

    // move ctor
    explicit Avl(Avl<T> &&move): root(nullptr)
    {
        this->root = move.root;
        move.root = nullptr;
    }

    // copy operator
    Avl &operator=(const Avl<T> &copy)
    {
        if (this == &copy) {
            return *this;
        }
        this->deleteTree(root);
        root = nullptr;
        copy.InOrder([&](const auto &value) { this->Insert(value); });
        return *this;
    }

    // move operator
    Avl &operator=(Avl<T> &&move) noexcept
    {
        this->deleteTree(root);
        this->root = move.root;
        move.root = nullptr;
        return *this;
    }

    // eql operator
    bool operator==(const Avl<T> &other) { return this->root == other.root; }

    // not eql operator
    bool operator!=(const Avl<T> &other) { return this->root != other.root; }

    // destructor
    virtual ~Avl()
    {
        deleteTree(root);
        root = nullptr;
    }

    /**
     * @brief inOrder traversal of the tree
     * @param callback
     */
    void InOrder(std::function<void(const T &)> callback) const
    {
        node *idx = root;
        InOrder(callback, idx);
    } // O(n) = n

    private:
    /**
     * @brief inOrder traversal of the tree
     * @param callBack
     * @param idx
     */
    void InOrder(std::function<void(const T &)> &callBack, node *idx) const
    {
        if (!idx)
            return;
        if (idx->lhs)
            InOrder(callBack, idx->lhs);
        callBack(*idx->data);
        if (idx->rhs)
            InOrder(callBack, idx->rhs);
    } // O(n) = n

    public:
    /**
     * @brief Find if the given item exist in the tree
     * @param data - the item to look for
     * @return true if it is found or false otherwise
     */
    bool Find(const T &data)
    {
        for (node *idx = root; idx;) {
            if ((*idx->data) == data)
                return true;
            else if ((*idx->data) < data)
                idx = idx->rhs;
            else
                idx = idx->lhs;
        }
        return false;
    } // O(n) = n log(n)

    void Insert(const T &item)
    {
        //             T    *data = new T(item);
        T *data = std::allocator_traits<Allocator>::allocate(_allocator, 1);
        std::allocator_traits<Allocator>::construct(_allocator, data, item);
        Insert(data);
    }

    /**
     * @brief add a new node to the tree
     * @param data - the new item to add
     */
    virtual void Insert(T *data)
    {
        if (!data) // Special case, given pointer is null
            return;
        //node *new_node = new(std::nothrow) node(data);
        node *new_node = std::allocator_traits<NodeAllocator>::allocate(_nodeAllocator, 1);
        if (!new_node)
            throw std::bad_alloc();
        std::allocator_traits<NodeAllocator>::construct(_nodeAllocator, new_node, data);
        new_node->rhs = nullptr;
        new_node->lhs = nullptr;
        if (!root) { // Special case, three is empty add new data as root
            root = new_node;
            computeBalance(root);
            return;
        }
        { // Regular case, allocate and find the right place to add a leaf
            node *parent = root;
            node *idx = root;
            while (idx) {
                if (*idx->data == *new_node->data) { // Special case the node already exist
                    return;
                }
                if (*idx->data < *new_node->data) {
                    parent = idx;
                    idx = idx->rhs;
                }
                else {
                    parent = idx;
                    idx = idx->lhs;
                }
            } // the node is found in parent add new_node as a leaf
            if (*parent->data < *new_node->data) { // Regular case, if new_data is bigger add right
                parent->rhs = new_node;
            }
            else { // Regular case, if new_data is smaller add left
                parent->lhs = new_node;
            }
            new_node->parent = parent;
            new_node->lhs = nullptr;
            new_node->rhs = nullptr;
        } // end regular case context
        // Now we can balance stuff here
        computeBalance(new_node->parent);
    }

    /**
     * @brief remove the given item from the tree.
     */
    void Remove(const T &item)
    {
        node *idx = root;
        if (!idx) // Special case, tree is empty
            return;
        while (idx) { // find the node to remove
            if (*idx->data == item) {
                break;
            }
            else if (*idx->data < item) {
                idx = idx->rhs;
            }
            else {
                idx = idx->lhs;
            }
        }
        if (!idx) // Special case this data is not in this tree
            return;
        node *new_root;
        node *parent = idx->parent;
        {                            // Regular case, delete the node and set correctly the new root
            if (parent == nullptr) { // Special case, idx is root
                new_root = deleteNode(idx);
                root = new_root;
            }
            else if (parent->lhs == idx) {
                new_root = deleteNode(idx);
                parent->lhs = new_root;
            }
            else {
                new_root = deleteNode(idx);
                parent->rhs = new_root;
            }
        } // end regular case context
        // Now we can balance thing here
        if (new_root) // if a sub tree is returned, start balancing here
            computeBalance(new_root);
        else if (
            parent) // if no subtree is returned start balancing at parent, if parent is null do nothing
            computeBalance(parent);
    }

    private:
    /**
     * @brief Delete the entire tree.
     */
    void deleteTree(node *item)
    {
        if (!item)
            return;
        deleteTree(item->rhs);
        deleteTree(item->lhs);
        std::allocator_traits<Allocator>::destroy(_allocator, item->data);
        std::allocator_traits<Allocator>::deallocate(_allocator, item->data, 1);
        std::allocator_traits<NodeAllocator>::destroy(_nodeAllocator, item);
        std::allocator_traits<NodeAllocator>::deallocate(_nodeAllocator, item, 1);
        //delete item;
    }

    /**
     * @brief Delete a node and return the root of the new subtree
     * @param item is the node to be deleted.
     * @return The root of the tree.
     */
    node *deleteNode(node *item)
    {
        node *new_root = nullptr;
        if (!item->lhs && !item->rhs) { // Regular case is the node is leaf
            std::allocator_traits<Allocator>::destroy(_allocator, item->data);
            std::allocator_traits<Allocator>::deallocate(_allocator, item->data, 1);
            std::allocator_traits<NodeAllocator>::destroy(_nodeAllocator, item);
            std::allocator_traits<NodeAllocator>::deallocate(_nodeAllocator, item, 1);
            //delete item;
            return new_root;
        }
        if (item->lhs && item->rhs) { // Regular case, item is a node with two child
            node *idx = item->rhs;
            while (idx->rhs) // Step 1, find the bigger elem in the right sub-tree
                idx = idx->rhs;
            std::allocator_traits<Allocator>::destroy(_allocator, item->data);
            std::allocator_traits<Allocator>::deallocate(_allocator, item->data, 1);
            //delete item->data;                    // Step 2, Swap item and idx data
            item->data = idx->data;
            idx->data = nullptr;
            node *idx_parent = idx->parent; // Step 4, remove the bigger elem initial node
            idx_parent->rhs = deleteNode(idx);
            if (idx_parent->rhs) // Step 5, make sure the parent link is correct
                idx_parent->rhs->parent = idx_parent;
            return item; // Step 6, return the initial node with the data field updated
        }
        else if (item->lhs) { // Regular case, item is a node with only lhs
            new_root = item->lhs;
            new_root->parent = item->parent;
            std::allocator_traits<Allocator>::destroy(_allocator, item->data);
            std::allocator_traits<Allocator>::deallocate(_allocator, item->data, 1);
            std::allocator_traits<NodeAllocator>::destroy(_nodeAllocator, item);
            std::allocator_traits<NodeAllocator>::deallocate(_nodeAllocator, item, 1);
            //delete item;
            return new_root;
        }
        else if (item->rhs) { // Regular case, item is node with only rhs
            new_root = item->rhs;
            new_root->parent = item->parent;
            std::allocator_traits<Allocator>::destroy(_allocator, item->data);
            std::allocator_traits<Allocator>::deallocate(_allocator, item->data, 1);
            std::allocator_traits<NodeAllocator>::destroy(_nodeAllocator, item);
            std::allocator_traits<NodeAllocator>::deallocate(_nodeAllocator, item, 1);
            //delete item;
            return new_root;
        }
        return nullptr; // should never be reach
    }

    node *    root;       /**< The root node of the tree container. */
    Allocator _allocator; /**< The default standard allocator. Only changed for debugging purpose.*/
    NodeAllocator _nodeAllocator; /** The node allocator. It is based on the provide allocator. */
};

} // namespace Netero
