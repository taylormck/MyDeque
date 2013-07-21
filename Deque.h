// ----------------------
// projects/deque/Deque.h
// Copyright (C) 2013
// Glenn P. Downing
// ----------------------

#ifndef Deque_h
#define Deque_h

#include <iostream>

#include <algorithm> // copy, equal, lexicographical_compare, max, swap
#include <cassert>   // assert
#include <iterator>  // iterator, bidirectional_iterator_tag
#include <memory>    // allocator
#include <stdexcept> // out_of_range
#include <utility>   // !=, <=, >, >=

using std::rel_ops::operator!=;
using std::rel_ops::operator<=;
using std::rel_ops::operator>;
using std::rel_ops::operator>=;

template<typename A, typename BI>
BI destroy(A& a, BI b, BI e) {
	while (b != e) {
		--e;
		a.destroy(&*e);
	}
	return b;
}

template<typename A, typename II, typename BI>
BI uninitialized_copy(A& a, II b, II e, BI x) {
	BI p = x;
	try {
		while (b != e) {
			a.construct(&*x, *b);
			++b;
			++x;
		}
	}
	catch (...) {
		destroy(a, p, x);
		throw;
	}
	return x;
}

template<typename A, typename BI, typename U>
BI uninitialized_fill(A& a, BI b, BI e, const U& v) {
	BI p = b;
	try {
		while (b != e) {
			a.construct(&*b, v);
			++b;
		}
	}
	catch (...) {
		destroy(a, p, b);
		throw;
	}
	return e;
}

template<typename T, typename A = std::allocator<T> >
class MyDeque {
	public:
		typedef A allocator_type;
		typedef typename allocator_type::value_type value_type;

		typedef typename allocator_type::size_type size_type;
		typedef typename allocator_type::difference_type difference_type;

		typedef typename allocator_type::pointer pointer;
		typedef typename allocator_type::const_pointer const_pointer;

		typedef typename allocator_type::reference reference;
		typedef typename allocator_type::const_reference const_reference;

		typedef typename allocator_type::template rebind<pointer>::other map_allocator_type;
        typedef typename map_allocator_type::pointer map_pointer;

	public:
		/**
		 * Compares the deques for equality
		 */
		friend bool operator ==(const MyDeque& lhs, const MyDeque& rhs) {
            if (lhs.size() != rhs.size())
                return false;
			return std::equal(lhs.myBegin, lhs.myEnd, rhs.myBegin);
		}

		/**
		 * Returns true if lhs is lexicographically less than rhs
		 */
		friend bool operator <(const MyDeque& lhs, const MyDeque& rhs) {
            if (lhs.size() < rhs.size())
                return true;
            return std::lexicographical_compare(lhs.myBegin, lhs.myEnd,
                                                rhs.myBegin, rhs.myEnd);
		}

    public:
        class iterator;

	private:
		const static unsigned int LOG_ROW_SIZE = 7;
		const static difference_type ROW_SIZE = 1 << LOG_ROW_SIZE;

		size_type mySize;
		size_type myMapSize;
		
		allocator_type myAllocator;
		map_allocator_type myMapAllocator;
		
		map_pointer myMap;

        iterator myBegin;
        iterator myEnd;

        iterator totalBegin;
        iterator totalEnd;

	private:

		bool valid() const {
			if (myMap == NULL)
				return false;
            if (!myBegin.valid())
				return false;
			if (!myEnd.valid())
				return false;
			if (!totalBegin.valid())
				return false;
			if (!totalEnd.valid())
				return false;
			if (myBegin > myEnd)
				return false;
			if (myBegin < totalBegin)
				return false;
			if (myEnd > totalEnd)
				return false;
            if (myMapSize < 1)
                return false;

			return true;
		}

        /**
         * Helper function to allocate one row
         */
        pointer allocateRow() {
            return myAllocator.allocate(ROW_SIZE);
        }

        /**
         * Helper function to deallocate one row
         */
        void deallocateRow(pointer row) {
        	myAllocator.deallocate(row, ROW_SIZE);
        }

        /**
         * Helper function to allocate a map
         */
        map_pointer allocateMap(size_type n) {
            return myMapAllocator.allocate(n);
        }

        /**
         * Helper function to deallocate a map
         */
        void deallocateMap(map_pointer map, size_type n) {
            myMapAllocator.deallocate(map, n);
        }

        /**
         * Helper function to initialize the memory
         */
         void initMap() {
         	myMap = allocateMap(1);
         	*myMap = allocateRow();
			totalBegin = iterator(*myMap, myMap);
			myBegin = totalBegin + ROW_SIZE / 2;
			myEnd = myBegin;
			totalEnd = totalBegin + ROW_SIZE;
			myMapSize = 1;
         }

        /**
         * Helper function to shrink the deque
         * Will leave leading values, cuts trailing values
         */
        void resizeMapSmaller(size_type n, map_pointer newMap) {
        	assert(n < myMapSize);
        	const map_pointer beginNewMap = newMap;

            // Trim off the empty space at the beginning
            map_pointer firstFilledRow = myBegin.currentRow;
            map_pointer b = myMap;
            map_pointer e = myMap + n;
            while(b != firstFilledRow) {
            	deallocateRow(*b++);
            }

            // Copy the rows into the new map
            map_pointer endCopy = std::copy(b, e, newMap);

            // Destroy the objects that won't fit
            iterator beginDestroy = iterator(*e, e);
            if (beginDestroy < myEnd) {
            	myEnd = destroy (myAllocator, beginDestroy, myEnd);
            }
            difference_type endOffset = (myEnd.currentRow - b);

            // Remove trailing empty rows
            const map_pointer endMap = myMap + myMapSize;
            while (e < endMap)
            	deallocateRow(*e++);

            // Fill in trailing empty rows
            const map_pointer endNewMap = newMap + n;
            while (endCopy < endNewMap)
            	*endCopy++ = allocateRow();

            // Fix begin and end iterators
            myBegin.setRow(beginNewMap);
        	myEnd.setRow(beginNewMap + endOffset);

            assert(myBegin.valid());    
            assert(myEnd.valid());
        }

        /**
         * Allocates more space for this deque
         * Attempts to place existing data in the middle
         */
        void resizeMapLarger(size_type n, map_pointer newMap) {
        	assert(n > myMapSize);
            map_pointer firstStop = newMap + ((n - myMapSize) / 2);
            map_pointer e = newMap + n;

            while(newMap != firstStop)
                *newMap++ = allocateRow();
            newMap = std::copy(myMap, myMap + myMapSize, newMap);
            while(newMap != e)
                *newMap++ = allocateRow();

            // Fix iterators
            difference_type beginOffset = myBegin.currentRow - totalBegin.currentRow;
            difference_type endOffset = myEnd.currentRow - totalBegin.currentRow;
		    myBegin.setRow(firstStop + beginOffset);
            myEnd.setRow(firstStop + endOffset);
            assert(myBegin.valid());
            assert(myEnd.valid());
        }

        /**
         * Helper function to resize the map, allocating/deallocating
         * more rows as necessary
         */
        void resizeMap(size_type n) {
            assert(n >= 0);
            assert(valid());
        	// Case 1, we are already size n
        	if (n == myMapSize)
        		return;

        	// Build new map
            const map_pointer newMap = allocateMap(n);

        	// Case 2, we are larger than size n
        	if (n < myMapSize)
        		resizeMapSmaller(n, newMap);
            else
            	resizeMapLarger(n, newMap);
            
            // Destroy old map
            deallocateMap(myMap, myMapSize);

            // Finally swap the maps and delete the old one
            myMap = newMap;
            myMapSize = n;

            // Set the total iterators and assert validity
            totalBegin = iterator(*myMap, myMap);
            totalEnd = iterator(*(myMap + n), myMap + n);
            assert(valid());
        }

        /**
         * Triples the size of the map
         */
        void growMap() {
        	assert(valid());
        	resizeMap(3 * myMapSize);
        }

        /**
         * Deletes the map and all the data
         */
        void clearMap() {
            destroy(myAllocator, myBegin, myEnd);
            for (map_pointer i = myMap; i < myMap + myMapSize; ++i)
            	deallocateRow(*i);
            deallocateMap(myMap, myMapSize);
        }

	public:

        // These are constant time +=, so I could use it later
		class iterator {
			public:
                typedef std::bidirectional_iterator_tag   iterator_category;
                typedef typename MyDeque::value_type      value_type;
                typedef typename MyDeque::difference_type difference_type;
                typedef typename MyDeque::pointer         pointer;
                typedef typename MyDeque::reference       reference;

				/**
				 * Compares the iterators for equality
				 */
				friend bool operator ==(const iterator& lhs, const iterator& rhs) {
					return (lhs.currentItem == rhs.currentItem);
				}

				/**
				 * Copmares the iterators for inequality
				 */
				friend bool operator !=(const iterator& lhs, const iterator& rhs) {
					return !(lhs == rhs);
				}

                /**
                 * Returns true if lhs is before rhs
                 *
                 * This wasn't required, but I thought I might need it
                 */
                friend bool operator < (const iterator& lhs, const iterator& rhs) {
                    // Compare rows first
                    // if they're equal, compare items
                    return (lhs.currentRow == rhs.currentRow) ?
                            (lhs.currentItem < rhs.currentItem):
                            (lhs.currentRow < lhs.currentRow);
                }

				/**
				 * Move the iterator rhs steps forward
				 */
				friend iterator operator +(iterator lhs, difference_type rhs) {
					return lhs += rhs;
				}

				/**
				 * Move the iterator rhs steps back
				 */
				friend iterator operator -(iterator lhs, difference_type rhs) {
					return lhs -= rhs;
				}

			private:
                pointer currentItem;
                map_pointer currentRow;
                pointer rowBegin;
                pointer rowEnd;

			private:

                bool valid() const {
                	
                    if (currentRow == NULL)
                        return false;
                    if (currentItem < rowBegin)
                        return false;
                    if (currentItem >= rowEnd)
                        return false;
                    return true;
                }

                void setRow(map_pointer newRow) {
                    currentRow = newRow;
                    rowBegin = *newRow;
                    rowEnd = rowBegin + ROW_SIZE;
                }

			public:
                /**
                 * Creates an empty iterator
                 * Does NOT create a valid iterator
                 */
                iterator() : currentItem(NULL), currentRow(NULL), rowBegin(NULL), rowEnd(NULL) {
                    assert(!valid());
                }

				/**
				 * Creates a new iterator using a pointer to the object and the row
				 */
				iterator(pointer item, map_pointer row) : currentItem(item), currentRow(row), rowBegin(*row), rowEnd(rowBegin + ROW_SIZE) {
                    assert(valid());
                }

				/**
				 * Return the object this iterator points to
				 */
				reference operator *() const {
					return *currentItem;
				}

				/**
				 * Call methods from the object this iterator points to
				 */
				pointer operator ->() const {
					return &**this;
				}

				/**
				 * Move this iterator forward by 1
				 */
				iterator& operator ++() {
					*this += 1;
					assert(valid());
					return *this;
				}

				/**
				 * Move this iterator forward by 1,
				 * returns the old value
				 */
				iterator operator ++(int) {
					iterator x = *this;
					++(*this);
					assert(valid());
					return x;
				}

				/**
				 * Move this iterator back by 1
				 */
				iterator& operator --() {
					*this -= 1;
					assert(valid());
					return *this;
				}

				/**
				 * Move this iterator back by 1,
				 * returns the old value
				 */
				iterator operator --(int) {
					iterator x = *this;
					--(*this);
					assert(valid());
					return x;
				}

				/**
				 * Move the iterator forward by d steps
				 */
				iterator& operator +=(difference_type d) {
					assert(valid());

                    difference_type newPosition = d + (currentItem - rowBegin);

                    // Same row
                    if (newPosition >= 0 && newPosition < ROW_SIZE)
                        currentItem += d;
                    else {
                        // Set the offset to the new row
                        // If 
                        difference_type newRow = newPosition > 0 ?
                            newPosition / ROW_SIZE :
                            -((-newPosition - 1) / ROW_SIZE) - 1;

                       	difference_type offset = newPosition - newRow * ROW_SIZE;
                        setRow(currentRow + newRow);
                        currentItem = rowBegin + offset;
                    }
					assert(valid());
					return *this;
				}

				/**
				 * Move the iterator back by d steps
				 */
				iterator& operator -=(difference_type d) {
					*this += -d;
					assert(valid());
					return *this;
				}
		};

	public:
		class const_iterator {
			public:
				typedef std::bidirectional_iterator_tag iterator_category;
				typedef typename MyDeque::value_type value_type;
				typedef typename MyDeque::difference_type difference_type;
				typedef typename MyDeque::const_pointer pointer;
				typedef typename MyDeque::const_reference reference;

			public:
				/**
				 * Compare the iterators for equality
				 */
				friend bool operator ==(const const_iterator& lhs, const const_iterator& rhs) {
                    return (lhs.currentItem == rhs.currentItem);
				}

				/**
				 * Compare the iterators for inequality
				 */
				friend bool operator !=(const const_iterator& lhs, const const_iterator& rhs) {
					return !(lhs == rhs);
				}

                /**
                 * Returns true if lhs is before rhs
                 */
                friend bool operator < (const const_iterator& lhs, const const_iterator& rhs) {
                    // Compare rows first
                    // if they're equal, compare items
                    return (lhs.currentRow == rhs.currentRow) ?
                            (lhs.currentItem < rhs.currentItem):
                            (lhs.currentRow < lhs.currentRow);
                }

				/**
				 * Move this iterator forward by rhs steps
				 */
				friend const_iterator operator +(const_iterator lhs, difference_type rhs) {
					return lhs += rhs;
				}

				/**
				 * Move this iterator back by rhs steps
				 */
				friend const_iterator operator -(const_iterator lhs, difference_type rhs) {
					return lhs -= rhs;
				}

			private:
                pointer currentItem;
                map_pointer currentRow;
                pointer rowBegin;
                pointer rowEnd;

			private:
                bool valid() const {
                    if (currentRow == NULL)
                        return false;
                    if (currentItem < rowBegin)
                        return false;
                    if (currentItem >= rowEnd)
                        return false;
                    return true;
                }

                void setRow(map_pointer newRow) {
                    currentRow = newRow;
                    rowBegin = *newRow;
                    rowEnd = rowBegin + ROW_SIZE;
                }

			public:
				/**
				 * Create a new const_iterator using a pointer to the data type
				 * and its row
				 */
				const_iterator(pointer item, map_pointer row) :
                        currentItem(item), currentRow(row), rowBegin(*row), rowEnd(rowBegin + ROW_SIZE) {
                    assert(valid());
                }

                /**
                 * Create a const_iterator using an iterator
                 */
                const_iterator(iterator rhs) :
                        currentItem(rhs.currentItem), currentRow(rhs.currentRow),
                        rowBegin(rhs.rowBegin), rowEnd(rhs.rowBegin + ROW_SIZE) {
                    assert(valid());
                }

				/**
				 * Return the object this iterator points to
				 */
				reference operator *() const {
                    return *currentItem;
				}

				/**
				 * Call methods from the object this iterator points to
				 */
				pointer operator ->() const {
					return &**this;
				}

				/**
				 * Move this iterator forward by 1
				 */
				const_iterator& operator ++() {
                    *this += 1;
                    assert(valid());
                    return *this;
				}

				/**
				 * Move this iterator forward by 1,
				 * returns the old value
				 */
				const_iterator operator ++(int) {
					const_iterator x = *this;
					++(*this);
					assert(valid());
					return x;
				}

				/**
				 * Move this iterator back by 1
				 */
				const_iterator& operator --() {
                    *this += -1;
                    assert(valid());
                    return *this;
				}

				/**
				 * Move this iterator back by 1,
				 * returns the old value
				 */
				const_iterator operator --(int) {
					const_iterator x = *this;
					--(*this);
					assert(valid());
					return x;
				}

				/**
				 * Move this iterator forward by d steps
				 */
				const_iterator& operator +=(difference_type d) {
                    assert(valid());
                    difference_type newPosition = d + (currentItem - rowBegin);

                    // Same row
                    if (newPosition >= 0 && newPosition < ROW_SIZE)
                        currentItem += d;
                    else {
                        // Move to lower row
                        difference_type newRow = newPosition > 0 ?
                            newPosition / ROW_SIZE :
                            -((-newPosition - 1) / ROW_SIZE) - 1;
                        setRow(currentRow + newRow);
                        currentItem = rowBegin + (newPosition - newRow * ROW_SIZE);
                    }
                    assert(valid());
                    return *this;
				}

				/**
				 * Move this iterator back by d steps
				 */
				const_iterator& operator -=(difference_type d) {
                    *this += -d;
                    assert(valid());
                    return *this;
				}
		};

	public:
		/**
		 * Create an empty MyDeque,
		 * has a minimum 1 row
		 */
		explicit MyDeque(const allocator_type& a = allocator_type()) :
				mySize(0),
				myMapSize(0),
				myAllocator(a),
				myMapAllocator(),
				myMap(NULL) {
			initMap();
			assert(valid());
		}

		/**
		 * Create a MyDeque of the specified size and fill with the specified
		 * values. Minimum one row
		 */
		explicit MyDeque(size_type s, const_reference v = value_type(), const allocator_type& a = allocator_type()) :
				mySize(0),
				myMapSize(0),
				myAllocator(a),
				myMapAllocator(),
				myMap(NULL) {
			initMap();
			resizeMap((s / ROW_SIZE) + 1);
            mySize = s;
			myBegin = iterator(*myMap, myMap);
			myEnd = uninitialized_fill(myAllocator, myBegin, myBegin + s, v);
			assert(valid());
		}

		/**
		 * Copy construct this MyDeque using another
		 */
		MyDeque(const MyDeque& that) :
				mySize(0),
				myMapSize(0),
				myAllocator(that.myAllocator),
				myMapAllocator(that.myMapAllocator),
				myMap(NULL) {
			initMap();
            resizeMap(that.myMapSize);
            mySize = that.mySize;
            myBegin = iterator(*myMap, myMap);
            myEnd = myBegin + mySize;
            myEnd = uninitialized_copy(myAllocator, that.myBegin, that.myEnd, myBegin);
            assert(valid());
		}

		/**
		 * Erase all data from the deque, releasing the memory
		 */
		~MyDeque() {
            clearMap();
		}

		/**
		 * Set this MyDeque equal to another
		 */
		MyDeque& operator =(const MyDeque& rhs) {
            clear();
            resizeMap(rhs.myMapSize);
            mySize = rhs.mySize;
            myBegin = iterator(*myMap, myMap);
            myEnd = uninitialized_copy(myAllocator, rhs.myBegin, rhs.myEnd, myBegin);

			assert(valid());
			return *this;
		}

		/**
		 * Index this MyDeque, return the indexth element
		 */
		reference operator [](size_type index) {
            return *(myBegin + index);
		}

		/**
		 * Index this MyDeque, return the indexth element
		 */
		const_reference operator [](size_type index) const {
			return const_cast<MyDeque*>(this)->operator[](index);
		}

		/**
		 * Gets the indexth element from the MyDeque
		 */
		reference at(size_type index) {
			return *(myBegin + index);
		}

		/**
		 * Gets the indexth element from the MyDeque
		 */
		const_reference at(size_type index) const {
			return const_cast<MyDeque*>(this)->at(index);
		}

		/**
		 * Returns the last element in the MyDeque
		 */
		reference back() {
            return *(myEnd - 1);
		}

		/**
		 * Returns the last element in the MyDeque
		 */
		const_reference back() const {
			return const_cast<MyDeque*>(this)->back();
		}

		/**
		 * Returns the first element in the MyDeque
		 */
		iterator begin() {
            return myBegin;
		}

		/**
		 * Returns the first element in the MyDeque
		 */
		const_iterator begin() const {
            return begin();
		}

		/**
		 * Delete all data from the MyDeque
		 */
		void clear() {
			clearMap();
			initMap();
			mySize = 0;
			assert(valid());
		}

		/**
		 * Returns true if this MyDeque is empty,
		 * false otherwise
		 */
		bool empty() const {
			return !size();
		}

		/**
		 * Returns an iterator to the space after the last element
		 */
		iterator end() {
            return myEnd;
		}

		/**
		 * Returns an iterator to the space after the last element
		 */
		const_iterator end() const {
            return end();
		}

		/**
		 * Remove the element pointed to by i
		 */
		iterator erase(iterator i) {
			if (i == myBegin) {
				pop_front();
				return myBegin;
			}
			else if (i == myEnd) {
				pop_back();
				return myEnd;
			}
			iterator j = i + 1;
			*i = *j;
			erase(j);
			assert(valid());
			return i;
		}

		/**
		 * Returns the first element in this Deque
		 */
		reference front() {
            return *myBegin;
		}

		/**
		 * Returns the first element in this Deque
		 */
		const_reference front() const {
			return const_cast<MyDeque*>(this)->front();
		}

		/**
		 * Insert an element into this iterator
		 */
		iterator insert(iterator i , const_reference v) {
			if (i == myBegin) {
				push_front(v);
				return myBegin;
			}
			else if (i == myEnd) {
				push_back(v);
				return myEnd;
			}
			const_reference tmp = *i;
			*i = v;
			insert(i + 1, tmp);
			assert(valid());
			return i;
		}

		/**
		 * Remove the last element in this MyDeque
		 */
		void pop_back() {
			--mySize;
			--myEnd;
			myAllocator.destroy(&*myEnd);
			assert(valid());
		}

		/**
		 * Remove the first element in this MyDeque
		 */
		void pop_front() {
			--mySize;
			myAllocator.destroy(&*myBegin);
			++myBegin;
			assert(valid());
		}

		/**
		 * Append an element the end of this MyDeque
		 */
		void push_back(const_reference v) {
            if(myEnd == totalEnd)
                growMap();
            myAllocator.construct(&*myEnd, v);
            ++myEnd;
			++mySize;
			assert(valid());
		}

		/**
		 * Append an element to the front of this MyDeque
		 */
		void push_front(const_reference v) {
			assert(valid());
            if(myBegin == totalBegin)
                growMap();
            --myBegin;
            myAllocator.construct(&*myBegin, v);
			++mySize;
			assert(valid());
		}

		/**
		 * Resize this MyDeque
		 * If s is larger than the current size, all new objects will
		 * be set equal to v.
		 * If s is smaller than the current size, then all trailing objects
		 * will be deleted
		 */
		void resize(size_type s, const_reference v = value_type()) {
			// TODO <your code>
			mySize = s;
			assert(valid());
		}

		/**
		 * Return the size of this MyDeque
		 */
		size_type size() const {
			return mySize;
		}

		/**
		 * Swap the contents of this deque and another
		 */
		void swap(MyDeque& other) {
			// TODO <your code>
			assert(valid());
		}
};

#endif // Deque_h
