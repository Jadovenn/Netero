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

     template <class T,
     		typename = std::enable_if<std::is_copy_constructible<T>::value>>
     class avl {

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

             int    getDepth() {
                 if (!rhs && !lhs)
                     return 0;
                 if (!rhs)
                     return lhs->getDepth();
                 else if (!lhs)
                     return rhs->getDepth();
                 else {
                     int    x = lhs->getDepth();
                     int    y = rhs->getDepth();
                     return x > y ? x : y;
                 }
             }

             void   computeBalance() {
                 int    x = rhs ? rhs->getDepth() : -1;
                 int    y = lhs ? lhs->getDepth() : -1;
                 balance = x - y;
             }

             int    balance;
             node   *parent;
             node   *rhs;
             node   *lhs;
             T  	*data;
         };

     public:
     	class iterator: public std::iterator<std::input_iterator_tag, T, T, const T*, T&> {
		public:
     		explicit iterator(avl &tree) {
     			tree.inOrder([&] (const auto &e) {
     				list.push_back(e);
     			});
     			idx = list.front();
     			list.pop_front();
     		}

     		explicit iterator(T	last) {
     			idx = last;
     		}

     		iterator    &operator++() {
     			if (!list.empty()) {
					idx = list.front();
					list.pop_front();
				}
     			return *this;
     		}

     		const T& operator*() const {
     			return idx;
     		}

     		bool    operator==(iterator other) const {
     			return idx == other.idx;
     		}

     		bool    operator!=(iterator other) const {
     			return idx != other.idx;
     		}

		private:
     		T				idx;
     		std::list<T>	list;
     	};
     	friend iterator;
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

     	~avl() {
     		deleteTree(root);
     	}

     	/**
     	 * @brief inOrder traversal of the tree
     	 * @param callback
     	 */
     	void	inOrder(std::function<void(const T&)> callback) {
     		node	*idx = root;
			inOrder(callback, idx);
     	}

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
     	}
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


#ifdef NETERO_DEBUG
		void	display() {
         	std::cout << "--Display-----------------" << std::endl;
         	node *idx = root;
         	inOrderTraversal(idx);
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
         node   *root;
     };

 }
