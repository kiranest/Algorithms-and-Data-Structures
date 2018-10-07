#ifndef RPOVDGBQN9TIEO3P
#define RPOVDGBQN9TIEO3P

#include "Exception.h"
#include "ece250.h"
#include <cassert>

template <typename Type>
class Search_tree {
	public:
		class Iterator;

	private:
		class Node {
			public:
                //Member variables
				Type node_value;
				int tree_height;

				// The left and right sub-trees
				Node *left_tree;
				Node *right_tree;

				// Previous and next nodes
				Node *previous_node;
				Node *next_node;

				// Member functions
				Node( Type const & = Type() );

                void update_height();

                int height() const;
				bool is_leaf() const;
				Node *front();
				Node *back();
				Node *find( Type const &obj );

				void clear();
                void balanceLeft( Node *&to_this );
                void balanceRight( Node *&to_this );
				bool insert( Type const &obj, Node *&to_this );
				bool erase( Type const &obj, Node *&to_this );
		};
        // Search_tree member variables
		Node *root_node;
		int tree_size;

		// Sentinels for the linearly ordered nodes
		Node *front_sentinel;
		Node *back_sentinel;

	public:
		class Iterator {
			private:
                // Member variables
				Search_tree *containing_tree;
				Node *current_node;
				bool is_end;

				// The constructor is private so that only the search tree can create an iterator
				Iterator( Search_tree *tree, Node *starting_node );

			public:
                //Member functions
				Type operator*() const;
				Iterator &operator++();
				Iterator &operator--();
				bool operator==( Iterator const &rhs ) const;
				bool operator!=( Iterator const &rhs ) const;

			// Make the search tree a friend so that it can call the constructor
			friend class Search_tree;
		};
        // Constructor and Destructor
		Search_tree();
		~Search_tree();

        // Member Functions
		bool empty() const;
		int size() const;
		int height() const;

		Type front() const;
		Type back() const;

		Iterator begin();
		Iterator end();
		Iterator rbegin();
		Iterator rend();
		Iterator find( Type const & );

		void clear();
		bool insert( Type const & );
		bool erase( Type const & );

	// Friends

	template <typename T>
	friend std::ostream &operator<<( std::ostream &, Search_tree<T> const & );
};

//////////////////////////////////////////////////////////////////////
//                Search Tree Public Member Functions               //
//////////////////////////////////////////////////////////////////////

// Search tree constructor
template <typename Type>
Search_tree<Type>::Search_tree():
root_node( nullptr ),
tree_size( 0 ),
front_sentinel( new Search_tree::Node( Type() ) ),
back_sentinel( new Search_tree::Node( Type() ) ) {
    // Point the sentinels at each other when the tree is empty
	front_sentinel->next_node = back_sentinel;
	back_sentinel->previous_node = front_sentinel;
}

// Search tree destructor
template <typename Type>
Search_tree<Type>::~Search_tree() {
	clear();
	delete front_sentinel;
	delete back_sentinel;
}

// Returns true when the tree contains no nodes
template <typename Type>
bool Search_tree<Type>::empty() const {
	return ( root_node == nullptr );
}

// Returns the number of nodes in the tree
template <typename Type>
int Search_tree<Type>::size() const {
	return tree_size;
}

// Returns the height of the tree
template <typename Type>
int Search_tree<Type>::height() const {
	return root_node->height();
}

// Returns the lowest value in the tree
template <typename Type>
Type Search_tree<Type>::front() const {
	if ( empty() ) {
		throw underflow();
	}

	return root_node->front()->node_value;
}

// Returns the highest value in the tree
template <typename Type>
Type Search_tree<Type>::back() const {
	if ( empty() ) {
		throw underflow();
	}

	return root_node->back()->node_value;
}

// Returns an iterator with a node pointer to the lowest value node if not empty
// If the tree is empty, it returns an iterator with end()
template <typename Type>
typename Search_tree<Type>::Iterator Search_tree<Type>::begin() {
	return empty() ? Iterator( this, back_sentinel ) : Iterator( this, root_node->front() );
}

// Returns an iterator with a node pointer to the back sentinel
template <typename Type>
typename Search_tree<Type>::Iterator Search_tree<Type>::end() {
	return Iterator( this, back_sentinel );
}

// Returns an iterator with a node pointer to the highest value node if not empty
// If the tree is empty, it returns an iterator with rend()
template <typename Type>
typename Search_tree<Type>::Iterator Search_tree<Type>::rbegin() {
	return empty() ? Iterator( this, front_sentinel ) : Iterator( this, root_node->back() );
}

// Returns an iterator with a node pointer to the front sentinel
template <typename Type>
typename Search_tree<Type>::Iterator Search_tree<Type>::rend() {
	return Iterator( this, front_sentinel );
}

// Returns an iterator with a node pointer that contains the argument object if found
// If the object can't be found or if the tree is empty, node pointer will be pointing to the back_sentinel
template <typename Type>
typename Search_tree<Type>::Iterator Search_tree<Type>::find( Type const &obj ) {
	if ( empty() ) {
		return Iterator( this, back_sentinel );
	}

	typename Search_tree<Type>::Node *search_result = root_node->find( obj );

	if ( search_result == nullptr ) {
		return Iterator( this, back_sentinel );
	} else {
		return Iterator( this, search_result );
	}
}

// Delete all nodes with the exception of the front and back sentinels in the tree
// Sets the tree size back to 0 and points the front and back sentinels to each other
template <typename Type>
void Search_tree<Type>::clear() {
	if ( !empty() ) {
		root_node->clear();
		root_node = nullptr;
		tree_size = 0;
	}

	// Reinitialize the sentinels
	front_sentinel->next_node = back_sentinel;
	back_sentinel->previous_node = front_sentinel;
}

// Inserts an object into the binary search tree
// If the tree is initially empty, point the front and back nodes to the root node and vice versa
// If the tree already contains the object, return false, otherwise return true
template <typename Type>
bool Search_tree<Type>::insert( Type const &obj ) {
	if ( empty() ) {
		root_node = new Search_tree::Node( obj );
		tree_size = 1;
		front_sentinel->next_node = root_node;
        root_node->previous_node = front_sentinel;
		back_sentinel->previous_node = root_node;
        root_node->next_node = back_sentinel;

		return true;
	} else if ( root_node->insert( obj, root_node ) ) {
		++tree_size;
		return true;
	} else {
		return false;
	}
}

// Erase the argument object in the tree, decrement the tree size, and return true
// If the tree doesn't contain the object, return false, otherwise return true
template <typename Type>
bool Search_tree<Type>::erase( Type const &obj ) {
	if ( !empty() && root_node->erase( obj, root_node ) ) {
		--tree_size;
		return true;
	} else {
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
//                   Node Public Member Functions                   //
//////////////////////////////////////////////////////////////////////

// Node constructor
template <typename Type>
Search_tree<Type>::Node::Node( Type const &obj ):
node_value( obj ),
left_tree( nullptr ),
right_tree( nullptr ),
next_node( nullptr ),
previous_node( nullptr ),
tree_height( 0 ) {
	// does nothing
}

// Update the height of the current node by getting the max height of the children and adding 1
template <typename Type>
void Search_tree<Type>::Node::update_height() {
	tree_height = std::max( left_tree->height(), right_tree->height() ) + 1;
}

// Return the tree height, if this node is empty, return the height as -1
template <typename Type>
int Search_tree<Type>::Node::height() const {
	return ( this == nullptr ) ? -1 : tree_height;
}

// Return true if the current node is a leaf node, false otherwise
// Returns true when both children are empty
template <typename Type>
bool Search_tree<Type>::Node::is_leaf() const {
	return ( (left_tree == nullptr) && (right_tree == nullptr) );
}

// Return a pointer to the front node
// Recursive function that continues to check if there is a left tree of the current node
template <typename Type>
typename Search_tree<Type>::Node *Search_tree<Type>::Node::front() {
	return ( left_tree == nullptr ) ? this : left_tree->front();
}

// Return a pointer to the back node
// Recursive function that continues to check if there is a right tree of the current node
template <typename Type>
typename Search_tree<Type>::Node *Search_tree<Type>::Node::back() {
	return ( right_tree == nullptr ) ? this : right_tree->back();
}

// Returns a pointer to the node that contains the argument object, or nullptr if it can't be found in the tree
// Recursively searches the tree
template <typename Type>
typename Search_tree<Type>::Node *Search_tree<Type>::Node::find( Type const &obj ) {
	if ( obj == node_value ) {
		return this;
	} else if ( obj < node_value ) {
		return (left_tree == nullptr) ? nullptr : left_tree->find( obj );
	} else {
		return ( right_tree == nullptr ) ? nullptr : right_tree->find( obj );
	}
}

// Recursively clear the tree
template <typename Type>
void Search_tree<Type>::Node::clear() {
	if ( left_tree != nullptr ) {
		left_tree->clear();
	}

	if ( right_tree != nullptr ) {
		right_tree->clear();
	}

	delete this;
}

// AVL balancing for cases where the left tree is taller than the right tree by a height of 2
template <typename Type>
void Search_tree<Type>::Node::balanceLeft( Search_tree<Type>::Node *&to_this ) {
    // If the left tree is empty then left AVL balancing is not required
    if(left_tree == nullptr){
        return;
    }
    // Check for a height imbalance and do the proper balancing
    else if(right_tree == nullptr || (left_tree->height() - right_tree->height() == 2)){
        // Case 1: Left-left imbalance
        if (left_tree->left_tree->height() > left_tree->right_tree->height()){
            Search_tree<Type>::Node *b = left_tree,
                    *BR = b->right_tree;
            b->right_tree = this;
            to_this = b;
            left_tree =  BR;
        }
        // Case 2: Left-right imbalance
        else if (left_tree->left_tree->height() < left_tree->right_tree->height()){
            Search_tree<Type>::Node *b  = left_tree,
                    *d  = b->right_tree,
                    *DL = d->left_tree,
                    *DR = d->right_tree;
            d->left_tree = b;
            d->right_tree = this;
            to_this = d;
            b->right_tree = DL;
            this->left_tree = DR;
        }
    }
}

// AVL balancing for cases where the right tree is taller than the left tree by a height of 2
template <typename Type>
void Search_tree<Type>::Node::balanceRight( Search_tree<Type>::Node *&to_this ) {
    // If the right tree is empty then right AVL balancing is not required
    if(right_tree == nullptr){
        return;
    }
    // Check for a height imbalance and do the proper balancing
    else if(left_tree == nullptr || (right_tree->height() - left_tree->height() == 2)) {
        // Case 1: Right-right imbalance
        if (right_tree->right_tree->height() > right_tree->left_tree->height()) {
            Search_tree<Type>::Node *b = right_tree,
                    *BL = b->left_tree;
            b->left_tree = this;
            to_this = b;
            right_tree = BL;
        }
        //Case 2: Right-left imbalance
        else if(right_tree->right_tree->height() < right_tree->left_tree->height()){
            Search_tree<Type>::Node *b = right_tree,
                    *d = b->right_tree,
                    *DL = d->left_tree,
                    *DR = d->right_tree;
            d->right_tree = b;
            d->left_tree = this;
            to_this = d;
            b->left_tree = DR;
            this->right_tree = DL;
        }
    }
}

// Insert an object into the proper node
template <typename Type>
bool Search_tree<Type>::Node::insert( Type const &obj, Search_tree<Type>::Node *&to_this ) {
	if ( obj < node_value ) {
        // The object is smaller than the current object and the left tree is empty
        // Insert at the left tree and update the next and previous nodes of the nodes around the object
		if ( left_tree == nullptr ) {
			left_tree = new Search_tree<Type>::Node( obj );
            this->previous_node->next_node = left_tree;
            left_tree->previous_node = this->previous_node;
			this->previous_node = left_tree;
            left_tree->next_node = this;
			update_height();
			return true;
		} else {
            // If the left tree isn't empty then an insertion could cause an imbalance
            // Therefore call balanceLeft() if inserting is successful
			if ( left_tree->insert( obj, left_tree ) ) {
                balanceLeft( to_this );
                update_height();
				return true;
			} else {
				return false;
			}
		}
	} else if ( obj > node_value ) {
        // The object is bigger than the current object and the right tree is empty
        // Insert at the right tree and update the next and previous nodes of the nodes around the object
		if ( right_tree == nullptr ) {
			right_tree = new Search_tree<Type>::Node( obj );
            this->next_node->previous_node = right_tree;
            right_tree->next_node = this->next_node;
			this->next_node = right_tree;
			right_tree->previous_node = this;
            update_height();

			return true;
		} else {
            // If the right tree isn't empty then an insertion could cause an imbalance
            // Therefore call balanceRight() if inserting is successful
			if ( right_tree->insert( obj, right_tree ) ) {
                balanceRight( to_this );
                update_height();
                return true;
			} else {
				return false;
			}
		}
    // If the argument object is already in the tree, return false
	} else {
		return false;
	}
}

// Erase the argument object from the tree
template <typename Type>
bool Search_tree<Type>::Node::erase( Type const &obj, Search_tree<Type>::Node *&to_this ) {
	if ( obj < node_value ) {
        // The object being less than the current object and the left tree being empty must mean the object wasn't in the tree
		if ( left_tree == nullptr ) {
			return false;
		} else {
            // If the left tree is not empty then a successful erase could mean the tree becomes imbalanced
            // A left erase calls for balanceRight() since the right tree will be taller than the left if there is an imbalance
			if ( left_tree->erase( obj, left_tree ) ) {
                balanceRight( to_this );
				update_height();
				return true;
			}

			return false;
		}
	} else if ( obj > node_value ) {
        // The object being bigger than the current object and the right tree being empty must mean the object wasn't in the tree
        if ( right_tree == nullptr ) {
			return false;
		} else {
            // If the right tree is not empty then a successful erase could mean the tree becomes imbalanced
            // A right erase calls for balanceLeft() since the left tree will be taller than the right if there is an imbalance
			if ( right_tree->erase( obj, right_tree ) ) {
                balanceLeft( to_this );
				update_height();
				return true;
			}

			return false;
		}
	} else {
        //Update all previous and next nodes around the erased node

        assert( obj == node_value );
        // Delete the current node pointer to null if the object was found in a leaf node
		if ( is_leaf() ) {
			to_this = nullptr;
            this->previous_node->next_node = this->next_node;
            this->next_node->previous_node = this->previous_node;
			delete this;
		}
        // If the node to be erased has a left tree that is empty move the right tree in its position before deleting
        else if ( left_tree == nullptr ) {
			to_this = right_tree;
            this->previous_node->next_node = this->next_node;
            this->next_node->previous_node = this->previous_node;
			delete this;
		}
        // If the node to be erased has a right tree that is empty move the left tree in its position before deleting
        else if ( right_tree == nullptr ) {
			to_this = left_tree;
            this->previous_node->next_node = this->next_node;
            this->next_node->previous_node = this->previous_node;
			delete this;
		}
        // If both child nodes are present, move the smallest node in the right tree to this position
        // Erase the node that the object used to be in
        else {
			node_value = right_tree->front()->node_value;
			right_tree->erase( node_value, right_tree );
			update_height();
		}

		return true;
	}
}

//////////////////////////////////////////////////////////////////////
//                   Iterator Private Constructor                   //
//////////////////////////////////////////////////////////////////////

template <typename Type>
Search_tree<Type>::Iterator::Iterator( Search_tree<Type> *tree, typename Search_tree<Type>::Node *starting_node ):
containing_tree( tree ),
current_node( starting_node ) {
	// Does nothing...
}

//////////////////////////////////////////////////////////////////////
//                 Iterator Public Member Functions                 //
//////////////////////////////////////////////////////////////////////

// Return the iterator's current node value
template <typename Type>
Type Search_tree<Type>::Iterator::operator*() const {
	return current_node->node_value;
}

// Update the current node to the node containing the next higher value
template <typename Type>
typename Search_tree<Type>::Iterator &Search_tree<Type>::Iterator::operator++() {
	// If we are already at end do nothing
    if(current_node != containing_tree->end().current_node)
        current_node = current_node->next_node;
	return *this;
}

// Update the current node to the node containing the next smaller value
template <typename Type>
typename Search_tree<Type>::Iterator &Search_tree<Type>::Iterator::operator--() {
	// If we are already at either rend, do nothing
	if(current_node->left_tree != containing_tree->rend().current_node)
		current_node = current_node->previous_node;

	return *this;
}

// Return true when the two iterators being compared have the same current_node, false otherwise
template <typename Type>
bool Search_tree<Type>::Iterator::operator==( typename Search_tree<Type>::Iterator const &rhs ) const {
	return ( current_node == rhs.current_node );
}

// Return false when the two iterators being compared have different current_node, false otherwise
template <typename Type>
bool Search_tree<Type>::Iterator::operator!=( typename Search_tree<Type>::Iterator const &rhs ) const {
	return ( current_node != rhs.current_node );
}

//////////////////////////////////////////////////////////////////////
//                            Friends                               //
//////////////////////////////////////////////////////////////////////

// You can modify this function however you want:  it will not be tested

template <typename T>
std::ostream &operator<<( std::ostream &out, Search_tree<T> const &list ) {
	out << "not yet implemented";

	return out;
}

#endif
