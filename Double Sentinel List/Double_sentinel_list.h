#ifndef DOUBLE_SENTINEL_LIST_H
#define DOUBLE_SENTINEL_LIST_H

#include <iostream>
#include "Exception.h"

template <typename Type>
class Double_sentinel_list {
	public:
		class Double_node {
			public:
				Double_node( Type const & = Type(), Double_node * = nullptr, Double_node * = nullptr );

				Type value() const;
				Double_node *previous() const;
				Double_node *next() const;

				Type         node_value;
				Double_node *previous_node;
				Double_node *next_node;
        };

		Double_sentinel_list();
		Double_sentinel_list( Double_sentinel_list const & );
		Double_sentinel_list( Double_sentinel_list && );
		~Double_sentinel_list();

		// Accessors

		int size() const;
		bool empty() const;

		Type front() const;
		Type back() const;

		Double_node *begin() const;
		Double_node *end() const;
		Double_node *rbegin() const;
		Double_node *rend() const;

		Double_node *find( Type const & ) const;
		int count( Type const & ) const;

		// Mutators

		void swap( Double_sentinel_list & );
		Double_sentinel_list &operator=( Double_sentinel_list );
		Double_sentinel_list &operator=( Double_sentinel_list && );

		void push_front( Type const & );
		void push_back( Type const & );

		void pop_front();
		void pop_back();

		int erase( Type const & );

	private:
		Double_node *list_head;
		Double_node *list_tail;
		int list_size;

		// List any additional private member functions you author here
	// Friends

	template <typename T>
	friend std::ostream &operator<<( std::ostream &, Double_sentinel_list<T> const & );
};

/////////////////////////////////////////////////////////////////////////
//                      Public member functions                        //
/////////////////////////////////////////////////////////////////////////

template <typename Type>
Double_sentinel_list<Type>::Double_sentinel_list():
// Updated the initialization list here
list_head( new Double_node() ),
list_tail( new Double_node() ),
list_size( 0 )
{
	//The empty constructors makes two sentinel nodes that point to each other since the list is empty
	list_head->next_node = list_tail;
	list_tail->previous_node = list_head;
}

template <typename Type>
Double_sentinel_list<Type>::Double_sentinel_list( Double_sentinel_list<Type> const &list ):
// Updated the initialization list here
list_head( new Double_node() ),
list_tail( new Double_node() ),
list_size( 0 )
{
	//The copy constructor copies the values of argument list without copying their addresses
	//push_back() is used so that the same order will remain
    list_head->next_node = list_tail;
    list_tail->previous_node = list_head;
    for(Double_node * ptr = list.begin(); ptr != list.end(); ptr = ptr->next()){
        push_back(ptr->value());
    }
}

template <typename Type>
Double_sentinel_list<Type>::Double_sentinel_list( Double_sentinel_list<Type> &&list ):
// Updated the initialization list here
list_head( new Double_node() ),
list_tail( new Double_node() ),
list_size( 0 )
{
	//The move constructor simply calls swap() on the argument list
	//swap() swaps the list_head, list_tail, and list_size variables between the current list and the argument list
	//The new list should have all the objects of the argument list, while in the argument list, the user
	//is expected to delete the objects
	swap(list);
}

template <typename Type>
Double_sentinel_list<Type>::~Double_sentinel_list() {
	//This function continuously deletes the front node until all elements are deleted and then deletes the memory
	//allocated for the sentinels
	while(!empty())
        pop_front();
    delete rend();
    delete end();
}

template <typename Type>
int Double_sentinel_list<Type>::size() const {
	//simply return the number of nodes in the list excluding sentinels
	return list_size;
}

template <typename Type>
bool Double_sentinel_list<Type>::empty() const {
	//returns whether the list has no nodes excluding sentinels
	return list_size == 0;
}

template <typename Type>
Type Double_sentinel_list<Type>::front() const {
	//This function returns the value of the first node that is not a sentinel.
	//If the list is empty, it throws an exception
	if(empty())
		throw underflow();

	return begin()->value(); //
}

template <typename Type>
Type Double_sentinel_list<Type>::back() const {
	//Similar to front(): returns the value of the last node that is not a sentinel.
	//Also throws exception for an empty list.
	if(empty())
		throw underflow();
	return rbegin()->value(); //
}

template <typename Type>
typename Double_sentinel_list<Type>::Double_node *Double_sentinel_list<Type>::begin() const {
	//Returns the pointer to the first object in the list that is not a sentinel
	return list_head->next();
}

template <typename Type>
typename Double_sentinel_list<Type>::Double_node *Double_sentinel_list<Type>::end() const {
	//Returns the pointer to the tail sentinel
	return list_tail;
}

template <typename Type>
typename Double_sentinel_list<Type>::Double_node *Double_sentinel_list<Type>::rbegin() const {
	//Returns the pointer to the last object in the list that is not a sentinel
	return list_tail->previous();
}

template <typename Type>
typename Double_sentinel_list<Type>::Double_node *Double_sentinel_list<Type>::rend() const {
	//Returns the address of the head sentinel
	return list_head;
}

template <typename Type>
typename Double_sentinel_list<Type>::Double_node *Double_sentinel_list<Type>::find( Type const &obj ) const {
	//Checks for the first found instance of the argument object in the list and returns its address.
	//Returns a nullpointer if the object is not in the list.
	for(Double_node * ptr = begin(); ptr != end(); ptr = ptr->next()){
        if(ptr->value() == obj)
            return ptr;
    }
	return end();
}

template <typename Type>
int Double_sentinel_list<Type>::count( Type const &obj ) const {
	//Counts the occurences of the argument object in the list and returns the count.
	int count = 0;
	for(Double_node * ptr = begin(); ptr != end(); ptr = ptr->next()){
		if(ptr->value() == obj)
			count++;
	}
	return count;
}

template <typename Type>
void Double_sentinel_list<Type>::swap( Double_sentinel_list<Type> &list ) {
	//Simply swaps the node member variables with the one in the argument list
	std::swap( this->list_head, list.list_head );
	std::swap( this->list_tail, list.list_tail );
	std::swap( this->list_size, list.list_size );
}

// The assignment operator
template <typename Type>
Double_sentinel_list<Type> &Double_sentinel_list<Type>::operator=( Double_sentinel_list<Type> rhs ) {
	// This is done for you
	swap( rhs );

	return *this;
}

// The move operator
template <typename Type>
Double_sentinel_list<Type> &Double_sentinel_list<Type>::operator=( Double_sentinel_list<Type> &&rhs ) {
	// This is done for you
	swap( rhs );

	return *this;
}

template <typename Type>
void Double_sentinel_list<Type>::push_front( Type const &obj ) {
	//Allocates new memory for a Double_node at the front of the list
	//Assigns the argument object as its value,
	//the head_sentinel as the previous pointer, and the current front object as its next node.
	//The current front node's previous_node value is changed first as it uses begin(), which
	//requires the head sentinel's next pointer to point to the current front node.
	Double_node * newFrontNode = new Double_node(obj, rend(), begin());
	begin()->previous_node = newFrontNode;
	rend()->next_node = newFrontNode;
    list_size++;
}

template <typename Type>
void Double_sentinel_list<Type>::push_back( Type const &obj ) {
	//Allocates new memory for a Double_node at the back of the list
	//Assigns the argument object as its value,
	//the last object as the previous pointer, and the current tail_sentinel as its next node.
	//The current tail node's next_node value is changed first as it uses rbegin(), which
	//requires the tail sentinel's previous pointer to point to the current tail node.
	Double_node * newBackNode = new Double_node(obj, rbegin(), end());
	rbegin()->next_node = newBackNode;
	end()->previous_node = newBackNode;
    list_size++;
}
template <typename Type>
void Double_sentinel_list<Type>::pop_front() {
	//Deletes the object at the front of the list if the list has at least 1 object; otherwise,
	//it throws an exception.
	//The head sentinel's next_node is set to point at the object after the one that is to be deleted
	//and vice versa.
	if(empty())
		throw underflow();
	Double_node * toPop = begin();
    begin()->next()->previous_node = rend();    //object after pop points back to head sentinel
	rend()->next_node = begin()->next();    //head sentinel now points to object after toPop
	delete toPop;
    list_size--;
}
template <typename Type>
void Double_sentinel_list<Type>::pop_back() {
	//Deletes the object at the back of the list if the list has at least 1 object; otherwise,
	//it throws an exception.
	//The tail sentinel's previous_node is set to point at the object before the one that is to be deleted
	//and vice versa.
	if(empty())
        throw underflow();
    Double_node * toPop = rbegin();
    rbegin()->previous()->next_node = end();
    end()->previous_node = rbegin()->previous();
    delete toPop;
    list_size--;
}
template <typename Type>
int Double_sentinel_list<Type>::erase( Type const &obj ) {
	//Deletes all nodes whose value matches that of the argument value.
	//A while loop is used as opposed to a for loop to allow the intended pointers to be deleted after the
	//traverse pointer has been updates as to avoid memory errors.
    int countDeleted = 0;
	Double_node * ptr = begin();
	while(ptr != end()){
		if(ptr->value() == obj){
			Double_node * toPop = ptr;
			ptr->previous()->next_node = ptr->next();
			ptr->next()->previous_node = ptr->previous();
			ptr = ptr->next();
			countDeleted++;
			list_size--;
			delete toPop;
		}
		else
			ptr = ptr->next();
	}
	return countDeleted;
}

template <typename Type>
Double_sentinel_list<Type>::Double_node::Double_node(
	Type const &nv,
	typename Double_sentinel_list<Type>::Double_node *pn,
	typename Double_sentinel_list<Type>::Double_node *nn ):
//Assigns the proper argument variables to the proper member variables
node_value( nv ),
previous_node( pn ),
next_node( nn )
{
	//empty constructor
}

template <typename Type>
Type Double_sentinel_list<Type>::Double_node::value() const {
	//Returns the value stored by the node
	return node_value;
}

template <typename Type>
typename Double_sentinel_list<Type>::Double_node *Double_sentinel_list<Type>::Double_node::previous() const {
	//Returns the pointer to the previous node
	return previous_node;
}

template <typename Type>
typename Double_sentinel_list<Type>::Double_node *Double_sentinel_list<Type>::Double_node::next() const {
	//Returns the pointer to the next node
	return next_node;
}

/////////////////////////////////////////////////////////////////////////
//                      Private member functions                       //
/////////////////////////////////////////////////////////////////////////

// If you author any additional private member functions, include them here

/////////////////////////////////////////////////////////////////////////
//                               Friends                               //
/////////////////////////////////////////////////////////////////////////

// You can modify this function however you want:  it will not be tested

template <typename T>
std::ostream &operator<<( std::ostream &out, Double_sentinel_list<T> const &list ) {
	out << "head";

	for ( typename Double_sentinel_list<T>::Double_node *ptr = list.rend(); ptr != nullptr; ptr = ptr->next() ) {
		if ( ptr == list.rend() || ptr == list.end() ) {
			out << "->S";
		} else {
			out << "->" << ptr->value();
		}
	}

	out << "->0" << std::endl << "tail";

	for ( typename Double_sentinel_list<T>::Double_node *ptr = list.end(); ptr != nullptr; ptr = ptr->previous() ) {
		if ( ptr == list.rend() || ptr == list.end() ) {
			out << "->S";
		} else {
			out << "->" << ptr->value();
		}
	}

	out << "->0";

	return out;
}

#endif
