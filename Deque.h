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
		 * TODO <your documentation>
		 */
		friend bool operator ==(const MyDeque& lhs, const MyDeque& rhs) {
			// TODO <your code>
			// you must use std::equal()
			return true;
		}

		/**
		 * TODO <your documentation>
		 */
		friend bool operator <(const MyDeque& lhs, const MyDeque& rhs) {
			// TODO <your code>
			// you must use std::lexicographical_compare()
			return true;
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

	private:

		bool valid() const {
			// TODO <your code>
			if (myMap == NULL)
				return false;
			if (myBegin > myEnd)
				return false;

			return true;
		}

        /**
         * TODO <your documentation>
         */
        pointer allocateRow() {
            return myAllocator.allocate(ROW_SIZE);
        }

        /**
         * TODO <your documentation>
         */
        void deallocateRow(pointer row) {
        	myAllocator.deallocate(row, ROW_SIZE);
        }

        /**
         * TODO <your documentation>
         */
        map_pointer allocateMap(size_type n) {
            return myMapAllocator.allocate(n);
        }

        /**
         * TODO <your documentation>
         */
        void deallocateMap(map_pointer map, size_type n) {
            myMapAllocator.deallocate(map, n);
        }

        /**
         * TODO <your documentation>
         */
        void resizeMap(size_type n) {
        	// Case 1, we are already size n
        	if (n == myMapSize)
        		return;

        	
        	// Case 2, we are larger than size n
        	if (n < myMapSize) {
                // TODO memory leak, need to deallocate rows
        		map_pointer b = myMap + n;
                map_pointer e = myMap + myMapSize;
                while (b != e)
                    deallocateRow(*b++);
                myMapSize = n;
                return;
        	}

        	// Case 3, n is larger than us
    		assert(n > myMapSize);

            const map_pointer newMap = allocateMap(n);

    		map_pointer b = newMap;
    		map_pointer firstStop = b + ((n - myMapSize) >> 1);
    		map_pointer e = newMap + n;

    		while(b != firstStop)
    			*b++ = allocateRow();
    		b = std::copy( myMap, myMap + myMapSize, b);
    		while(b != e)
    			*b++ = allocateRow();
        	deallocateMap(myMap, myMapSize);
        	myMap = newMap;
        	myMapSize = n;
        }

        /**
         * TODO <your documentation>
         */
        void growMap() {
        	resizeMap(3 * std::max(static_cast<size_type>(1), myMapSize));
        }

	public:

		class iterator {
			public:

				/**
				 * TODO <your documentation>
				 */
				friend bool operator ==(const iterator& lhs, const iterator& rhs) {\
					return ((lhs.currentRow == rhs.currentRow) &&
                        (lhs.currentItem == rhs.currentItem));
				}

				/**
				 * TODO <your documentation>
				 */
				friend bool operator !=(const iterator& lhs, const iterator& rhs) {
					return !(lhs == rhs);
				}

                /**
                 * TODO <your documentation>
                 */
                friend bool operator < (const iterator& lhs, const iterator& rhs) {
                    // Compare rows first
                    // if they're equal
                    return (lhs.currentRow == rhs.currentRow) ?
                            (lhs.currentItem < rhs.currentItem):
                            (lhs.currentRow < lhs.currentRow);
                }

                /**
                 * TODO <your documentation>
                 */
                friend bool operator <= (const iterator& lhs, const iterator& rhs) {
                    return !(rhs < lhs);
                }

                /**
                 * TODO <your documentation>
                 */
                friend bool operator > (const iterator& lhs, const iterator& rhs) {
                    return rhs < lhs;
                }

                /**
                 * TODO <your documentation>
                 */
                friend bool operator >= (const iterator& lhs, const iterator& rhs) {
                    return !(lhs < rhs);
                }

				/**
				 * TODO <your documentation>
				 */
				friend iterator operator +(iterator lhs, difference_type rhs) {
					return lhs += rhs;
				}

				/**
				 * TODO <your documentation>
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
					return ((currentRow != NULL)
                            && (currentItem >= rowBegin)
                            && (currentItem < rowEnd));
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
				 * TODO <your documentation>
				 */
				iterator(pointer item, map_pointer row) : currentItem(item), currentRow(row), rowBegin(*row), rowEnd(rowBegin + ROW_SIZE) {
                    assert(valid());
                }
				/**
				 * TODO <your documentation>
				 */
				reference operator *() const {
					// TODO <your code>
					// dummy is just to be able to compile the skeleton, remove it
					static value_type dummy;
					return dummy;
				}

				/**
				 * TODO <your documentation>
				 */
				pointer operator ->() const {
					return &**this;
				}

				/**
				 * TODO <your documentation>
				 */
				iterator& operator ++() {
					*this += 1;
					assert(valid());
					return *this;
				}

				/**
				 * TODO <your documentation>
				 */
				iterator operator ++(int) {
					iterator x = *this;
					++(*this);
					assert(valid());
					return x;
				}

				/**
				 * TODO <your documentation>
				 */
				iterator& operator --() {
					*this += -1;
					assert(valid());
					return *this;
				}

				/**
				 * TODO <your documentation>
				 */
				iterator operator --(int) {
					iterator x = *this;
					--(*this);
					assert(valid());
					return x;
				}

				/**
				 * TODO <your documentation>
				 */
				iterator& operator +=(difference_type d) {
					assert(valid());
                    difference_type newPosition = d + (currentItem - rowBegin);

                    // Same row
                    if (newPosition >= 0 && newPosition < ROW_SIZE)
                        currentItem += d;
                    else {
                        // Move to lower row
                        difference_type newRow;
                        if (newPosition > 0)
                            newRow = newPosition / ROW_SIZE;
                        // Move to higher row
                        else {
                            newRow = -difference_type((-newPosition - 1) / ROW_SIZE) - 1;
                        }
                        setRow(currentRow + newRow);
                        currentItem = rowBegin + (newPosition - newRow * ROW_SIZE);
                    }
					assert(valid());
					return *this;
				}

				/**
				 * TODO <your documentation>
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
				typedef std::random_access_iterator_tag iterator_category;
				typedef typename MyDeque::value_type value_type;
				typedef typename MyDeque::difference_type difference_type;
				typedef typename MyDeque::const_pointer pointer;
				typedef typename MyDeque::const_reference reference;

			public:
				/**
				 * TODO <your documentation>
				 */
				friend bool operator ==(const const_iterator& lhs, const const_iterator& rhs) {
                    return const_cast<const iterator>(lhs) == const_cast<const iterator>(rhs);
				}

				/**
				 * TODO <your documentation>
				 */
				friend bool operator !=(const const_iterator& lhs, const const_iterator& rhs) {
					return !(lhs == rhs);
				}

                /**
                 * TODO <your documentation>
                 */
                friend bool operator < (const const_iterator& lhs, const const_iterator& rhs) {
                    return const_cast<const iterator>(lhs) < const_cast<const iterator>(rhs);
                }

                /**
                 * TODO <your documentation>
                 */
                friend bool operator <= (const const_iterator& lhs, const const_iterator& rhs) {
                    return !(rhs < lhs);
                }

                /**
                 * TODO <your documentation>
                 */
                friend bool operator > (const const_iterator& lhs, const const_iterator& rhs) {
                    return rhs < lhs;
                }

                /**
                 * TODO <your documentation>
                 */
                friend bool operator >= (const const_iterator& lhs, const const_iterator& rhs) {
                    return !(lhs < rhs);
                }

				/**
				 * TODO <your documentation>
				 */
				friend const_iterator operator +(const_iterator lhs, difference_type rhs) {
					return lhs += rhs;
				}

				/**
				 * TODO <your documentation>
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
                    return const_cast<iterator*>(this)->valid();
				}

			public:
				/**
				 * TODO <your documentation>
				 */
				const_iterator(pointer item, map_pointer row) :
                        currentItem(item), currentRow(row), rowBegin(*row), rowEnd(rowBegin + ROW_SIZE) {
                    assert(valid());
                }

                /**
                 * TODO <your documentation>
                 */
                const_iterator(iterator rhs) :
                        currentItem(rhs.item), currentRow(rhs.row),
                        rowBegin(*(rhs.row)), rowEnd(rhs.rowBegin + ROW_SIZE) {
                    assert(valid());
                }

				/**
				 * TODO <your documentation>
				 */
				reference operator *() const {
                    return const_cast<iterator*>(this)->operator*();
				}

				/**
				 * TODO <your documentation>
				 */
				pointer operator ->() const {
					return &**this;
				}

				/**
				 * TODO <your documentation>
				 */
				const_iterator& operator ++() {
                    return const_cast<iterator*>(this)->operator++();
				}

				/**
				 * TODO <your documentation>
				 */
				const_iterator operator ++(int) {
					const_iterator x = *this;
					++(*this);
					assert(valid());
					return x;
				}

				/**
				 * TODO <your documentation>
				 */
				const_iterator& operator --() {
                    return const_cast<iterator*>(this)->operator--();
				}

				/**
				 * TODO <your documentation>
				 */
				const_iterator operator --(int) {
					const_iterator x = *this;
					--(*this);
					assert(valid());
					return x;
				}

				/**
				 * TODO <your documentation>
				 */
				const_iterator& operator +=(difference_type n) {
                    return const_cast<iterator*>(this)->operator+=(n);
				}

				/**
				 * TODO <your documentation>
				 */
				const_iterator& operator -=(difference_type n) {
                    return const_cast<iterator*>(this)->operator-=(n);
				}
		};

	public:
		/**
		 * TODO <your documentation>
		 */
		explicit MyDeque(const allocator_type& a = allocator_type()) :
				mySize(0),
				myMapSize(0),
				myAllocator(a),
				myMapAllocator(),
				myMap(NULL) {
			resizeMap(1);
			myBegin = iterator(*myMap + (ROW_SIZE >> 1), myMap);
			myEnd = myBegin;
			assert(valid());
		}

		/**
		 * TODO <your documentation>
		 */
		explicit MyDeque(size_type s, const_reference v = value_type(), const allocator_type& a = allocator_type()) :
				mySize(0),
				myMapSize(0),
				myAllocator(a),
				myMapAllocator(),
				myMap(NULL) {
			resizeMap((s / ROW_SIZE) + 1);
            mySize = s;
			myBegin = iterator(*myMap, myMap);
            assert(myBegin.valid());
			myEnd = uninitialized_fill(myAllocator, myBegin, myBegin + s, v);
			assert(valid());
		}

		/**
		 * TODO <your documentation>
		 */
		MyDeque(const MyDeque& that) :
				mySize(0),
				myMapSize(0),
				myAllocator(that.myAllocator),
				myMapAllocator(that.myMapAllocator),
				myMap(NULL) {
			// TODO <your code>
            resizeMap((that.mySize / ROW_SIZE) + 1);
            mySize = that.mySize;
			myBegin = iterator(*myMap, myMap);
            // assert(false);
            // myEnd = uninitialized_copy(myAllocator, that.myBegin, that.myEnd, myBegin);
            myEnd = myBegin + mySize;
            // assert(false);
			assert(valid());
            // assert(false);
		}

		/**
		 * TODO <your documentation>
		 */
		~MyDeque() {
			destroy(myAllocator, myBegin, myEnd);

			for (size_type k = 0; k < myMapSize; ++k)
				deallocateRow(*(myMap + k));

			deallocateMap(myMap, myMapSize);
			assert(valid());
		}

		/**
		 * TODO <your documentation>
		 */
		MyDeque& operator =(const MyDeque& rhs) {
			// TODO <your code>
			assert(valid());
			return *this;
		}

		/**
		 * TODO <your documentation>
		 */
		reference operator [](size_type index) {
			// TODO <your code>
			// dummy is just to be able to compile the skeleton, remove it
			static value_type dummy;
			return dummy;
		}

		/**
		 * TODO <your documentation>
		 */
		const_reference operator [](size_type index) const {
			return const_cast<MyDeque*>(this)->operator[](index);
		}

		/**
		 * TODO <your documentation>
		 */
		reference at(size_type index) {
			// TODO <your code>
			// dummy is just to be able to compile the skeleton, remove it
			return *(myBegin + index);
		}

		/**
		 * TODO <your documentation>
		 */
		const_reference at(size_type index) const {
			return const_cast<MyDeque*>(this)->at(index);
		}

		/**
		 * TODO <your documentation>
		 */
		reference back() {
			// TODO <your code>
			// dummy is just to be able to compile the skeleton, remove it
			static value_type dummy;
			return dummy;
		}

		/**
		 * TODO <your documentation>
		 */
		const_reference back() const {
			return const_cast<MyDeque*>(this)->back();
		}

		/**
		 * TODO <your documentation>
		 */
		iterator begin() {
			// TODO <your code>
			return iterator(/* <your arguments> */);
		}

		/**
		 * TODO <your documentation>
		 */
		const_iterator begin() const {
			// TODO <your code>

			return const_iterator(iterator());
		}

		/**
		 * TODO <your documentation>
		 */
		void clear() {
			// TODO <your code>
			assert(valid());
		}

		/**
		 * TODO <your documentation>
		 */
		bool empty() const {
			return !size();
		}

		/**
		 * TODO <your documentation>
		 */
		iterator end() {
			// TODO <your code>
			return iterator(/* <your arguments> */);
		}

		/**
		 * TODO <your documentation>
		 */
		const_iterator end() const {
			// TODO <your code>
			return const_iterator(iterator());
		}

		/**
		 * TODO <your documentation>
		 */
		iterator erase(iterator) {
			--mySize;
			// TODO <your code>
			assert(valid());
			return iterator();
		}

		/**
		 * TODO <your documentation>
		 */
		reference front() {
			// TODO <your code>
			// dummy is just to be able to compile the skeleton, remove it
			static value_type dummy;
			return dummy;
		}

		/**
		 * TODO <your documentation>
		 */
		const_reference front() const {
			return const_cast<MyDeque*>(this)->front();
		}

		/**
		 * TODO <your documentation>
		 */
		iterator insert(iterator, const_reference) {
			++mySize;
			// TODO <your code>
			// assert(valid());
			return iterator();
		}

		/**
		 * TODO <your documentation>
		 */
		void pop_back() {
			--mySize;
			// TODO <your code>
			assert(valid());
		}

		/**
		 * TODO <your documentation>
		 */
		void pop_front() {
			--mySize;
			// TODO <your code>
			assert(valid());
		}

		/**
		 * TODO <your documentation>
		 */
		void push_back(const_reference) {
			++mySize;
			// TODO <your code>
			assert(valid());
		}

		/**
		 * TODO <your documentation>
		 */
		void push_front(const_reference) {
			++mySize;
			// TODO <your code>
			assert(valid());
		}

		/**
		 * TODO <your documentation>
		 */
		void resize(size_type s, const_reference v = value_type()) {
			// TODO <your code>
			mySize = s;
			assert(valid());
		}

		/**
		 * TODO <your documentation>
		 */
		size_type size() const {
			return mySize;
		}

		/**
		 * TODO <your documentation>
		 */
		void swap(MyDeque&) {
			// TODO <your code>
			assert(valid());
		}
};

#endif // Deque_h
