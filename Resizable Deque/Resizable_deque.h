#ifndef DYNAMIC_DEQUE_H
#define DYNAMIC_DEQUE_H

#include "Exception.h"

template <typename Type>
class Resizable_deque {
	public:
		Resizable_deque( int = 16 );
		Resizable_deque( Resizable_deque const & );
		Resizable_deque( Resizable_deque && );
		~Resizable_deque();

		Type front() const;
		Type back() const;
		int size() const;
		bool empty() const;
		int capacity() const;

		void swap( Resizable_deque & );
		Resizable_deque &operator=( Resizable_deque const& );
		Resizable_deque &operator=( Resizable_deque && );
		void push_front( Type const & );
		void push_back( Type const & );
		void pop_front();
		void pop_back();
		void clear();

	private:
		int ifront;
		int iback;
		int deque_size;
		int initial_array_capacity;
		int array_capacity;
		Type *array;

	// Friends

	template <typename T>
	friend std::ostream &operator<<( std::ostream &, Resizable_deque<T> const & );
};

/////////////////////////////////////////////////////////////////////////
//                   Constructors and Destructors                      //
/////////////////////////////////////////////////////////////////////////

// Constructor
template <typename Type>
Resizable_deque<Type>::Resizable_deque( int n ):
ifront( 0 ),
iback( -1 ),
deque_size( 0 ),
initial_array_capacity( std::max(n, 16) ),          //sets the deque capacity at a minimum of 16
array_capacity( std::max(n,16) ),
array( new Type[array_capacity] )
{

}
// Copy Constructor
template <typename Type>
Resizable_deque<Type>::Resizable_deque( Resizable_deque const &deque ):
ifront( deque.ifront ),
iback( deque.iback ),
deque_size( deque.size() ),
initial_array_capacity( deque.initial_array_capacity ),
array_capacity( deque.capacity() ),
array( new Type[array_capacity] )
{
    //Loop through the argument array such that only the values important values are copied
    //instead of copying all the elements in the argument array.
    //Must a while loop since the condition of the loop is that the iterator != iback + 1, which it
    //does if ifront is right after iback in the argument array.
    int i = ifront;
    do {
        array[i] = deque.array[i];
        ++i;
        if(i == capacity())
            i = 0;
    } while(i != iback + 1);
}

// Move Constructor
template <typename Type>
Resizable_deque<Type>::Resizable_deque( Resizable_deque &&deque ):
ifront( 0 ),
iback( -1 ),
deque_size( 0 ),
initial_array_capacity( 16 ),
array_capacity( 16 ),
array( new Type[array_capacity] )
{
    //Simply call the swap function to swap all member variables with the argument deque.
	swap(deque);
}
// Destructor
template <typename Type>
Resizable_deque<Type>::~Resizable_deque() {
	delete [] array;
}

/////////////////////////////////////////////////////////////////////////
//                     Public Member Functions                         //
/////////////////////////////////////////////////////////////////////////

template <typename Type>
int Resizable_deque<Type>::size() const {
	return deque_size;
}

template <typename Type>
int Resizable_deque<Type>::capacity() const {
	return array_capacity;
}

template <typename Type>
bool Resizable_deque<Type>::empty() const {
	return deque_size == 0;
}

template <typename  Type>
Type Resizable_deque<Type>::front() const {
    //Only returns the object at the front if there is at least one value in the array, otherwise throws an exception.
	if(empty())
		throw underflow();

	return array[ifront];
}

template <typename  Type>
Type Resizable_deque<Type>::back() const {
    //Only returns the object at the back if there is at least one value in the array, otherwise throws an exception.
    if(empty())
		throw underflow();

	return array[iback];
}
//TODO
template <typename Type>
void Resizable_deque<Type>::swap( Resizable_deque<Type> &deque ) {
    //swap all member values with this deque and the argument deque
    std::swap(ifront, deque.ifront);
    std::swap(iback, deque.iback);
    std::swap(deque_size, deque.deque_size);
    std::swap(initial_array_capacity, deque.initial_array_capacity);
    std::swap(array_capacity, deque.array_capacity);
    std::swap(array, deque.array);
}

template <typename Type>
Resizable_deque<Type> &Resizable_deque<Type>::operator=( Resizable_deque<Type> const &rhs ) {
	Resizable_deque<Type> copy( rhs );
	swap( copy );

	return *this;
}

template <typename Type>
Resizable_deque<Type> &Resizable_deque<Type>::operator=( Resizable_deque<Type> &&rhs ) {
	swap( rhs );

	return *this;
}
template <typename Type>
void Resizable_deque<Type>::push_front( Type const &obj ) {
    //If the deque is not full, add the object at the front.
    //Otherwise, create a new array of double the capacity and copy over the objects from the old array.
    //The new array is indexed such that ifront is 0 and iback is the size of the old array.
    //Update the member variables, delete the old array, and set array to the new array.
    if(size() == capacity()){
        Type * tempArray = new Type[capacity()*2];
        tempArray[0] = obj;
        for(int i = 0; i < size() ; i++ ){
            tempArray[i+1] = array[ifront];
            ++ifront;
            if(ifront == capacity())
                ifront = 0;
        }
        ++deque_size;
        iback = size() - 1;                     //account for 0 indexing
        ifront = 0;
        array_capacity = capacity()*2;
        delete [] array;
        array = tempArray;
    }
    else {
        //Check if the element to be added will be the first. If it is the first, then iback must be updated
        //so that it is 0 instead of -1.
        //Otherwise, decrement ifront and insert the object at the index of ifront.
        //ifront is set to the max index if it is less than 0 when decremented.
        if(iback < 0) {
            iback = 0;
            array[ifront] = obj;
        }
        else {
            --ifront;
            if (ifront < 0)
                ifront = capacity() - 1;
            array[ifront] = obj;
        }
        ++deque_size;
    }
}
template <typename Type>
void Resizable_deque<Type>::push_back( Type const &obj ) {
    //Similar to push_front in terms of logic and code except the object is added to the back of the queue.
    if(size() == capacity()){
        Type * tempArray = new Type[capacity()*2];
        tempArray[size()] = obj;
        for(int i = 0; i < size() ; i++ ){
            tempArray[i] = array[ifront];
            ++ifront;
            if(ifront == capacity())
                ifront = 0;
        }
        ++deque_size;
        iback = size() - 1;
        ifront = 0;
        array_capacity = capacity()*2;
        delete [] array;
        array = tempArray;
    }
    else {
        if(iback < 0) {
            iback = 0;
            array[ifront] = obj;
        }
        else {
            ++iback;
            if (iback == capacity())
                iback = 0;
            array[iback] = obj;
        }
        ++deque_size;
    }
}
template <typename Type>
void Resizable_deque<Type>::pop_front() {
    //If empty throw an exception.
    //If not, point ifront to the next object and decrease the size by 1.
    //If ifront is at max index, set it to 0.
    //If the new size is a quarter of the current capacity and bigger than the initial capacity,
    //then make a new array of half the size, copy over the elements, update the member variables,
    //delete the old array, and set array to the new array.
    if(empty())
        throw underflow();
    ++ifront;
    --deque_size;
    if(ifront == capacity())
        ifront = 0;
    if(size() == capacity()/4 && capacity() > initial_array_capacity){
        Type * tempArray = new Type[capacity()/2];
        for(int i = 0; i < size() ; i++){
            tempArray[i] = array[ifront];
            ++ifront;
            if(ifront == capacity())
                ifront = 0;
        }
        array_capacity = capacity() / 2;
        ifront = 0;
        iback = size() - 1;
        delete [] array;
        array = tempArray;
    }
}
template <typename Type>
void Resizable_deque<Type>::pop_back() {
    //Similar to pop_front in terms of logic and code, except the object at the back is popped.
    if(empty())
        throw underflow();
    --iback;
    --deque_size;
    if(iback < 0)
        iback = capacity() - 1;
    if(size() == capacity()/4 && capacity() > initial_array_capacity){
        Type * tempArray = new Type[capacity()/2];
        for(int i = 0; i < size() ; i++){
            tempArray[i] = array[ifront];
            ++ifront;
            if(ifront == capacity())
                ifront = 0;
        }
        array_capacity = capacity() / 2;
        ifront = 0;
        iback = size() - 1;
        delete [] array;
        array = tempArray;
    }}
template <typename Type>
void Resizable_deque<Type>::clear() {
    //Set all member variables to the default value as if no object are present in the deque.
    //Also revert to the initial array capacity if it's not current capacity.
    ifront = 0;
    iback = -1;
    deque_size = 0;
    if(array_capacity != initial_array_capacity) {
        array_capacity = initial_array_capacity;
        delete [] array;
        array = new Type[initial_array_capacity];
    }
}
/////////////////////////////////////////////////////////////////////////
//                      Private member functions                       //
/////////////////////////////////////////////////////////////////////////

// Enter any private member functios (helper functions) here


/////////////////////////////////////////////////////////////////////////
//                               Friends                               //
/////////////////////////////////////////////////////////////////////////

// You can modify this function however you want:  it will not be tested

template <typename T>
std::ostream &operator<<( std::ostream &out, Resizable_deque<T> const &list ) {
	out << "not yet implemented";

	return out;
}

#endif
