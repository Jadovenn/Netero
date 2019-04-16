 /**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <type_traits>

 namespace netero {

 	/**
 	 * @brief Adelson Velsky Landis (AVL) tree
 	 * @tparam T the type hold by the container
 	 */
     template <class T,
     		typename = std::enable_if<std::is_copy_constructible<T>::value>>
     class avl {


     	/**
     	 * @brief structure representing a node in the tree,
     	 * it hold the data provide by the client
     	 */
         struct node {
             explicit node(T *data, node *parent = nullptr)
              : data(data),
                parent(parent),
                rhs(nullptr),
                lhs(nullptr),
                balance(0)
              {
                 computeBalance();
              }

              ~node() {
             	delete data;
             }

             /**
              * @brief compute the depth of a node recursivly
              * by descending to all child of the node
              * @return the depth
              */
             int    getDepth() {
                 if (!rhs && !lhs)
                     return 0;
                 if (!rhs)
                     return lhs->getDepth() + 1;
                 else if (!lhs)
                     return rhs->getDepth() + 1;
                 else {
                     int    x = lhs->getDepth() + 1;
                     int    y = rhs->getDepth() + 1;
                     return x > y ? x : y;
                 }
             }

             /**
              * @brief compute the balance of the node
              */
             void   computeBalance() {
                 int    x = rhs ? rhs->getDepth() : -1;
                 int    y = lhs ? lhs->getDepth() : -1;
                 balance = x - y;
                 if (parent)
                 	parent->computeBalance();
             }

             int    balance;
             node   *parent;
             node   *rhs;
             node   *lhs;
             T  	*data;
         };

     public:
     	/**
     	 * @brief iterator for the container to be compatible
     	 * with std function and for-range based loop
     	 */
     	class iterator: public std::iterator<std::input_iterator_tag, T, T, const T*, const T&> {
		public:
     		explicit iterator(avl &tree) {
     			tree.inOrder([&] (const auto &e) {
     				list.push_back(e);
     			});
     			idx = list.front();
     			list.pop_front();
     			isListEmpty = false;
     		}

     		explicit iterator(T	last) {
     			idx = last;
				isListEmpty = false;
     		}

     		iterator    &operator++() {
     			if (!list.empty()) {
					idx = list.front();
					list.pop_front();
				}
     			else {
     				isListEmpty = true;
     			}
     			return *this;
     		}

     		const T& operator*() const { return idx; }

     		bool    operator==(iterator other) const { return idx == other.idx && isListEmpty; }

     		bool    operator!=(iterator other) const { return !isListEmpty; }

		private:
     		bool 			isListEmpty;
     		T				idx;
     		std::list<T>	list;
     	};
     	friend iterator;
     	/**
     	 * @brief return an iterator to a list generated with traversal in order
     	 * of the tree
     	 * @return
     	 */
     	iterator	begin() {return iterator(*this);}
     	iterator	end() {
     		T	data;
     		this->inOrder([&] (const auto &e) {
     			data = e;
     		});
     		return iterator(data);
     	}

     	avl()
     	: root(nullptr) {};

     	~avl() { deleteTree(root); }

     	/**
     	 * @brief inOrder traversal of the tree
     	 * @param callback
     	 */
     	void	inOrder(std::function<void(const T&)> callback) {
     		node	*idx = root;
			inOrder(callback, idx);
     	} // O(n) = n

	 private:
     	/**
     	 * @brief inOrder traversal of the tree
     	 * @param callBack
     	 * @param idx
     	 */
     	void 	inOrder(std::function<void(const T&)> &callBack, node *idx) {
     		if (!idx)
     			return;
     		if (idx->lhs)
     			inOrder(callBack, idx->lhs);
			callBack(*idx->data);
     		if (idx->rhs)
     			inOrder(callBack, idx->rhs);
     	} // O(n) = n
	 public:
     	/**
     	 * @brief search if the given item exist in the tree
     	 * @param data - the item to look for
     	 * @return true if it is found or false otherwise
     	 */
     	bool	search(const T&data) {
			for (node *idx = root;idx;) {
				if ((*idx->data) == data)
					return true;
				else if ((*idx->data) < data)
					idx = idx->rhs;
				else
					idx = idx->lhs;
			}
			return false;
     	} // O(n) = n log(n)

     	void add(const T &item) {
     		T	*data = new T(item);
     		add(data);
     	}

     	/**
     	 * @brief add a new node to the tree
     	 * @param data - the new item to add
     	 */
         virtual void   add(T *data) {
             if (!data) // Special case, given pointer is null
                 return;
             node *new_node = new(std::nothrow) node(data);
             if (!new_node)
                 throw std::bad_alloc();
             new_node->rhs = nullptr;
             new_node->lhs = nullptr;
             if (!root) { // Special case, three is empty add new data as root
                 root = new_node;
                 root->computeBalance();
                 return;
             }
             { // Regular case, allocate and find the right place to add a leaf
                 node *parent = root;
                 node *idx = root;
                 while (idx) {
                 	if (*idx->data == *new_node->data) { // Special case the node already exist
						return;
                 	}
                 	else if (*idx->data < *new_node->data) {
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
             new_node->parent->computeBalance();
         }

         void 	remove(const T &item) {
         	node	*idx = root;
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
         	node	*new_root;
         	{ // Regular case, delete the node and set correctly the new root
         		if (idx->parent == nullptr) { // Special case, idx is root
         			new_root = deleteNode(idx);
         			root = new_root;
         		}
         		else if (idx->parent->lhs == idx) {
         			new_root = deleteNode(idx);
					idx->parent->lhs = new_root;
         		}
         		else {
         			new_root = deleteNode(idx);
					idx->parent->rhs = new_root;
         		}
			} // end regular case context
			// Now we can balance thing here
			if (new_root)
				new_root->computeBalance();
         }

#ifdef NETERO_DEBUG
		void	display() {
         	std::cout << "--Display-----------------" << std::endl;
         	if (root) {
				node *idx = root;
				inOrderTraversal(idx);
			}
         	else
         		std::cout << "[empty]" << std::endl;
			std::cout << "--------------------------" << std::endl;
        }

        void 	inOrderTraversal(node *idx) {
         	if (!idx)
         		return;
			inOrderTraversal(idx->lhs);
			std::cout << "data: " << *idx->data << ", parent: ";
			if (idx->parent) std::cout << *idx->parent->data; else std::cout << "nullptr";
			std::cout << ", lhs: ";
			if (idx->lhs) std::cout << *idx->lhs->data; else std::cout << "nullptr";
			std::cout << ", rhs: ";
			if (idx->rhs) std::cout << *idx->rhs->data; else std::cout << "nullptr";
			std::cout << ", balance: " ;
			std::cout << idx->balance << std::endl;
			inOrderTraversal(idx->rhs);
         }
#endif
     private:
     	void 	deleteTree(node *item) {
     		if (!item)
     			return;
     		deleteTree(item->rhs);
     		deleteTree(item->lhs);
     		delete item;
     	}

     	/**
     	 * @brief delete a node and return the root of the new subtree
     	 * @param data
     	 */
     	node 	*deleteNode(node *item) {
			node	*new_root = nullptr;
     		if (!item->lhs && !item->rhs) { // Regular case is the node is leaf
     			delete item;
     			return new_root;
     		}
     		if (item->lhs && item->rhs) { // Regular case, item is a node with two child
				node *idx = item->rhs;
				while (idx->rhs)					// Step 1, find the bigger elem in the right sub-tree
					idx = idx->rhs;
				delete item->data;					// Step 2, Swap item and idx data
				item->data = idx->data;
				idx->data = nullptr;
				node *idx_parent = idx->parent; 	// Step 4, remove the bigger elem initial node
				idx_parent->rhs = deleteNode(idx);
				if (idx_parent->rhs)				// Step 5, make sure the parent link is correct
					idx_parent->rhs->parent = idx_parent;
				return item;						// Step 6, return the initial node with the data field updated
     		}
     		else if (item->lhs) { // Regular case, item is a node with only lhs
     			new_root = item->lhs;
     			new_root->parent = item->parent;
     			delete item;
     			return new_root;
     		}
     		else if (item->rhs) { // Regular case, item is node with only rhs
     			new_root = item->rhs;
				new_root->parent = item->parent;
     			delete item;
     			return new_root;
     		}
     		return nullptr; // should never be reach
        }
         node   *root;
     };

 }
