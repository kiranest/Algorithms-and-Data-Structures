#ifndef DOUBLE_HASH_TABLE_H
#define DOUBLE_HASH_TABLE_H

#ifndef nullptr
#define nullptr 0
#endif

#include "Exception.h"
#include "ece250.h"

enum bin_state_t { UNOCCUPIED, OCCUPIED, ERASED };

template <typename Type>
class Quadratic_hash_table {
	private:
		int count;
        int countErased;
		int power;
		int array_size;
		int mask;
		Type *array;
		bin_state_t *occupied;

		int hash( Type const & ) const;

	public:
		Quadratic_hash_table( int = 5 );
		~Quadratic_hash_table();
		int size() const;
		int capacity() const;
		double load_factor() const;
		bool empty() const;
		bool member( Type const & ) const;
		Type bin( int ) const;

		void print() const;

		void insert( Type const & );
		bool erase( Type const & );
		void clear();

	// Friends

	template <typename T>
	friend std::ostream &operator<<( std::ostream &, Quadratic_hash_table<T> const & );
};

/////////////////////////////////////////////////////////////////////////
//                   Constructors and Destructors                      //
/////////////////////////////////////////////////////////////////////////

// Constructor
template <typename Type>
Quadratic_hash_table<Type>::Quadratic_hash_table( int m ):
count( 0 ), countErased( 0 ),
power( (m >= 0) ? m : 5),                       // The power of 2 will simply be the argument if positive, but 5 if negative
array_size( 1 << power ),
mask( array_size - 1 ),
array( new Type[array_size] ),
occupied( new bin_state_t[array_size] ) {

    // Initialize all hash tables to be unoccupied
	for ( int i = 0; i < array_size; ++i ) {
		occupied[i] = UNOCCUPIED;
	}
}

//Destructor
template  <typename Type>
Quadratic_hash_table<Type>::~Quadratic_hash_table(){
    delete [] array;
    delete [] occupied;
}

/////////////////////////////////////////////////////////////////////////
//                     Public Member Functions                         //
/////////////////////////////////////////////////////////////////////////

//ACCESSORS

// Return the number of elements stored in the hash table
template <typename Type>
int Quadratic_hash_table<Type>::size() const{
    return count;
}

// Return the number of bins in the hash table
template <typename Type>
int Quadratic_hash_table<Type>::capacity() const{
    return array_size;
}

// Return the load factor of the hash table
template <typename Type>
double Quadratic_hash_table<Type>::load_factor() const {
    return static_cast<double>(count + countErased) / static_cast<double>(array_size);
}

// Return true if the hash table is empty, false otherwise
template <typename Type>
bool Quadratic_hash_table<Type>::empty() const {
    return size() == 0;
}

// Return true if the argument object is in the hash table
template <typename Type>
bool Quadratic_hash_table<Type>::member( Type const &obj) const {
    // Get the hash value for the object that is to be found
    int bin = hash( obj );

    for(int k = 0; k < array_size; k++){
        //Quadratic probing
        bin = (bin + k) % array_size;

        // If an UNOCCUPIED bin is reached before finding the object, then the object can't be in the hash table
        if(occupied[bin] == UNOCCUPIED){
            return false;
        }

        // Only return true if the bin is OCCUPIED AND the argument object is equal to the array element
        if(occupied[bin] == OCCUPIED){
            if(array[bin] == obj)
                return true;
        }
    }

    return false;
}

// Return the content of bin n
template <typename Type>
Type Quadratic_hash_table<Type>::bin(int n) const {
    return array[n];
}

// Print contents in all OCCUPIED bins, null otherwise
template <typename Type>
void Quadratic_hash_table<Type>::print() const {
    for(int i = 0; i < capacity(); i++){
        std::cout << "bin(" << i << "): " << array[i] << std::endl;
    }
}

//MUTATORS

// Insert an object into the hash table
template <typename Type>
void Quadratic_hash_table<Type>::insert(Type const &obj) {
    // Throw exception if the hash table is full
    if(size() == capacity())
        throw overflow();
    // Do nothing if the hash table is not full and the argument object is already in the hash table
    if(count < array_size && member(obj))
        return;

    int bin = hash( obj );                         // Get the hash value for the object that is to be inserted
    for(int k = 0; k < array_size; k++){
        // Quadratic probing
        bin = (bin + k) % array_size;

        // Look for an unoccupied bin and insert into that bin
        if(occupied[bin] != OCCUPIED){
            array[bin] = obj;
            count++;
            // Decrement the countErased variable if the bin was previously erased
            if(occupied[bin] == ERASED)
                countErased--;
            occupied[bin] = OCCUPIED;
            return;
        }
    }
}

// Erases an object from the hash table and returns true on success, false otherwise
template <typename Type>
bool Quadratic_hash_table<Type>::erase(Type const &obj) {
    // Get the hash value for the object that is to be erased
    int bin = hash( obj );

    for(int k = 0; k < array_size; k++){
        // Quadratic probing
        bin = (bin + k) % array_size;

        // If an UNOCCUPIED bin is reached before finding the object, then the object can't be in the hash table
        if(occupied[bin] == UNOCCUPIED){
            return false;
        }
        // If the bin is OCCUPIED and the array contains the object, then set the bin to ERASED
        if(occupied[bin] == OCCUPIED) {
            if (array[bin] == obj) {
                occupied[bin] = ERASED;
                count--;
                countErased++;
                return true;
            }
        }
    }

    return false;

}

// Clear all elements in the hash table by setting all bins to UNOCCUPIED and setting the counts to 0
template <typename Type>
void Quadratic_hash_table<Type>::clear() {
    for (int i = 0; i < array_size; i++){
        occupied[i] = UNOCCUPIED;
    }
    count = 0;
    countErased = 0;
}

/////////////////////////////////////////////////////////////////////////
//                      Private member functions                       //
/////////////////////////////////////////////////////////////////////////


// Hash function that returns a hash value
template <typename Type>
int Quadratic_hash_table<Type>::hash( Type const &obj ) const {
    // Calculate the hash value by taking the object cast as an integer module M
    // Cast the object as an int to account for doubles
    int hash_value = static_cast<int>(obj) % array_size;

    // Return the hash value if it's positive
    // If the hash value is negative, return hash value + M
    return (hash_value >= 0) ? hash_value : hash_value + array_size;
}

template <typename T>
std::ostream &operator<<( std::ostream &out, Quadratic_hash_table<T> const &hash ) {
	for ( int i = 0; i < hash.capacity(); ++i ) {
		if ( hash.occupied[i] == UNOCCUPIED ) {
			out << "- ";
		} else if ( hash.occupied[i] == ERASED ) {
			out << "x ";
		} else {
			out << hash.array[i] << ' ';
		}
	}

	return out;
}

#endif
