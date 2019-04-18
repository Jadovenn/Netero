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
					   balance(0) {
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
			 int getDepth() {
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
			  * @brief compute the balance of a tree recursivly
			  * @details it start from the node of the first call
			  * and climb to the root of the tree, it does balance the tree
			  * if necessary
			  */
			 void computeBalance() {
			     this->singleBalance();
				 if (balance == 2 || balance == -2)
					 this->balanceTree(); // will call computeBalance again
				 else if (parent)
					 parent->computeBalance();
			 }

			 /**
			  * @brief compute the balance of a single node (rhs - lhs)
			  * @details instead of going recursivly to the root
			  * it only compute the node, it does not balance the tree
			  */
			 void	singleBalance() {
			 int x = rhs ? rhs->getDepth() : -1;
				 int y = lhs ? lhs->getDepth() : -1;
				 balance = x - y;
			 }

			 /**
			  * @brief balance the tree depending the balance factor
			  * @details case 1: (2)(1) the right subtree is heavy on right
			  *          case 2: (-2)(-1) the left subtree is heavy on left
			  *          case 3: (2)(-1) the right subtree as a left subtree heavy
			  *          case 4: (-2)(1) the left subtree as a right subtree heavy
			  */
			 void balanceTree() {
				 if (balance == 2) { // case 1 or 3
					 if (rhs->balance == 1) { // case 1
					 	rotateLeft(this);
					 	this->computeBalance();
					 } else if (rhs->balance == -1) { // case 3
					 	rotateRight(this->rhs);
					 	node *subtree = rotateLeft(this);
					 	subtree->lhs->singleBalance();
					 	subtree->rhs->computeBalance();
					 }
				 } else if (balance == -2) { // case 2 or 4
					 if (lhs->balance == -1) { // case 2
					 	rotateRight(this);
					 	this->computeBalance();
					 } else if (lhs->balance == 1) { // case 4
					 	rotateLeft(this->lhs);
					 	node *subtree = rotateRight(this);
					 	subtree->lhs->singleBalance();
					 	subtree->rhs->computeBalance();
					 }
				 }
			 }

			 /**
			  * @brief rotate the given tree to the right and return the new root of the subtree
			  * @param subtree - root of a subtree
			  * @return new root of the subtree
			  */
			 node *rotateRight(node *subtree) {
			 	node *new_root = subtree->lhs;
				new_root->parent = subtree->parent;
				subtree->parent = new_root;
				node *tmp_subRight = new_root->rhs;
				new_root->rhs = subtree;
				subtree->lhs = tmp_subRight;
				if (new_root->parent) {
					if (new_root->parent->rhs == subtree)
						new_root->parent->rhs = new_root;
					else if (new_root->parent->lhs == subtree)
						new_root->parent->lhs = new_root;
				}
				return new_root; // no balance recomputing at this point
			 }

             /**
              * @brief rotate the given tree to the left and return the new root of the subtree
              * @param subtree - root of a subtree
              * @return new root of the subtree
              */
			 node *rotateLeft(node *subtree) {
				 node *new_root = subtree->rhs;
				 new_root->parent = subtree->parent;
				 subtree->parent = new_root;
				 node *tmp_subLeft = new_root->lhs;
				 new_root->lhs = subtree;
				 subtree->rhs = tmp_subLeft;
				if (new_root->parent) {
					if (new_root->parent->rhs == subtree)
						new_root->parent->rhs = new_root;
					else if (new_root->parent->lhs == subtree)
						new_root->parent->lhs = new_root;
				}
				return new_root; // no balance recomputing at this point
			 }

			 int balance;
			 node *parent;
			 node *rhs;
			 node *lhs;
			 T *data;
		 };
     public:
     	/**
     	 * @brief iterator for the container to be compatible
     	 * with std function and for-range based loop
     	 * @details it build a list following a in-order traversal of the tree and iterate over it
     	 */
     	class iterator: public std::iterator<std::input_iterator_tag, T, T, const T*, const T&> {
		public:
     	    /**
     	     * @brief build an iterator from a tree and start at the bottom left
     	     * @param tree - to iterate over
     	     */
     		explicit iterator(avl &tree) {
     			tree.inOrder([&] (const auto &e) {
     				list.push_back(e);
     			});
     			idx = list.front();
     			list.pop_front();
     			isListEmpty = false;
     		}

     		/**
     		 * @brief build an iterator from a specific value
     		 * @details this iterator will consider this value as
     		 * a last value to iterate
     		 * @param last
     		 */
     		explicit iterator(T	last) {
     			idx = last;
				isListEmpty = false;
     		}

     		/**
     		 * @brief increment the iterator to the next value
     		 * @return
     		 */
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

     		/**
     		 * @brief return the index value
     		 * @return
     		 */
     		const T& operator*() const { return idx; }

     		/**
     		 * @brief eql comparator to another iterator
     		 * @param other - other iterator
     		 * @return true if equal, false otherwise
     		 */
     		bool    operator==(iterator other) const { return idx == other.idx && isListEmpty; }

     		/**
     		 * @brief noteql comparator to another iterator
     		 * @param other - other iterator
     		 * @return true if not eql, false otherwise
     		 */
     		bool    operator!=(iterator other) const { return !isListEmpty; }

		private:
     		bool 			isListEmpty;
     		T				idx;
     		std::list<T>	list;
     	};
     	friend iterator;
     	/**
     	 * @brief return an iterator to the begining of the
     	 * tree following in-order traversal
     	 * @return iterator
     	 */
     	iterator	begin() {return iterator(*this);}

     	/**
     	 * @brief return an iterator to the end of the tree
     	 * following the in-order traversal
     	 * @return iterator
     	 */
     	iterator	end() {
     		T	data;
     		this->inOrder([&] (const auto &e) {
     			data = e;
     		});
     		return iterator(data);
     	}

     	// default ctor
     	avl()
     	: root(nullptr) {};

     	// copy ctor
     	explicit avl(const avl<T> &copy) {

     	}

     	explicit avl(const avl<T> &&move) {

     	}

     	bool operator==(const avl<T> &other) {

     	}

     	bool operator!=(const avl<T> &other) {

     	}

     	// destructor
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
         	node	*parent = idx->parent;
         	{ // Regular case, delete the node and set correctly the new root
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
				new_root->computeBalance();
			else if (parent) // if no subtree is returned start balancing at parent, if parent is null do nothing
				parent->computeBalance();

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
