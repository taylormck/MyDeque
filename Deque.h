// ----------------------
// projects/deque/Deque.h
// Copyright (C) 2013
// Glenn P. Downing
// ----------------------

#ifndef Deque_h
#define Deque_h

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

	private:
		const static int LOG_ROW_SIZE = 7;
		const static size_type ROW_SIZE = 1 << LOG_ROW_SIZE;

		size_type mySize;
		size_type myMapSize;
		
		allocator_type myAllocator;
		map_allocator_type myMapAllocator;
		
		map_pointer myMap ;

	private:

		bool valid() const {
			// TODO <your code>
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
        void clearMap(map_pointer map, size_type n) {
            const map_pointer b = map;
            const map_pointer e = map + n;
            while (map != e) {
            	destroy(myAllocator, *map, *map + ROW_SIZE);
            	++map;
            }
            deallocateMap(b, n);
        }

        /**
         * TODO <your documentation>
         */
        void resizeMap(size_type n) {
            // TODO <your code>
            // const map_pointer newMap = allocateMap(n);
        }

        /**
         * TODO <your documentation>
         */
        void growMap() {
        	resizeMap(3 * myMapSize);
        }

	public:

		class iterator {
			public:

				typedef std::bidirectional_iterator_tag iterator_category;
				typedef typename MyDeque::value_type value_type;
				typedef typename MyDeque::difference_type difference_type;
				typedef typename MyDeque::pointer pointer;
				typedef typename MyDeque::reference reference;

			public:

				/**
				 * TODO <your documentation>
				 */
				friend bool operator ==(const iterator& lhs, const iterator& rhs) {
					// TODO <your code>
					return true;
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

				// TODO <your data>

			private:

				bool valid() const {
					// TODO <your code>
					return true;
				}

			public:

				/**
				 * TODO <your documentation>
				 */
				iterator(/* <your arguments> */) {
					// TODO <your code>
					// assert(valid());
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
					// TODO <your code>
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
					// TODO <your code>
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
					// TODO <your code>
					assert(valid());
					return *this;
				}

				/**
				 * TODO <your documentation>
				 */
				iterator& operator -=(difference_type d) {
					// TODO <your code>
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
				 * TODO <your documentation>
				 */
				friend bool operator ==(const const_iterator& lhs, const const_iterator& rhs) {
					// TODO <your code>
					return true;
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
				// TODO <your data>

			private:
				bool valid() const {
					// TODO <your code>
					return true;
				}

			public:
				/**
				 * TODO <your documentation>
				 */
				const_iterator(/* <your arguments> */) {
					// TODO <your code>
					assert(valid());
				}

                /**
                 * TODO <your documentation>
                 */
                const_iterator(iterator) {
                    // TODO <your code>
                    assert(valid());
                }

				// Default copy, destructor, and copy assignment.
				// const_iterator (const const_iterator&);
				// ~const_iterator ();
				// const_iterator& operator = (const const_iterator&);

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
				const_iterator& operator ++() {
					// TODO <your code>
					assert(valid());
					return *this;
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
					// TODO <your code>
					assert(valid());
					return *this;
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
				const_iterator& operator +=(difference_type) {
					// TODO <your code>
					assert(valid());
					return *this;
				}

				/**
				 * TODO <your documentation>
				 */
				const_iterator& operator -=(difference_type) {
					// TODO <your code>
					assert(valid());
					return *this;
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
			// TODO <your code>
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
			// TODO <your code>
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
			assert(valid());
		}

		/**
		 * TODO <your documentation>
		 */
		~MyDeque() {
			// TODO <your code>
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
			static value_type dummy;
			return dummy;
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
			return const_iterator(/* <your arguments> */);
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
			return const_iterator(/* <your arguments> */);
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
