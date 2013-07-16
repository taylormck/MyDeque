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

		typedef typename allocator_type::template rebind<pointer>::other pointer_allocator_type;

	public:
		/**
		 * <your documentation>
		 */
		friend bool operator ==(const MyDeque& lhs, const MyDeque& rhs) {
			// <your code>
			// you must use std::equal()
			return true;
		}

		/**
		 * <your documentation>
		 */
		friend bool operator <(const MyDeque& lhs, const MyDeque& rhs) {
			// <your code>
			// you must use std::lexicographical_compare()
			return true;
		}

	private:
		const static int LOG_INNER_SIZE = 5;
		const static size_type INNER_ARRAY_SIZE = 1 << LOG_INNER_SIZE;

		size_type mySize;
		size_type myOuterArraySize;
		
		allocator_type myAllocator;
		pointer_allocator_type myPointerAllocator;
		
		pointer myArray ;

	private:

		bool valid() const {
			// <your code>
			return true;
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
				 * <your documentation>
				 */
				friend bool operator ==(const iterator& lhs, const iterator& rhs) {
					// <your code>
					return true;
				}

				/**
				 * <your documentation>
				 */
				friend bool operator !=(const iterator& lhs, const iterator& rhs) {
					return !(lhs == rhs);
				}

				/**
				 * <your documentation>
				 */
				friend iterator operator +(iterator lhs, difference_type rhs) {
					return lhs += rhs;
				}

				/**
				 * <your documentation>
				 */
				friend iterator operator -(iterator lhs, difference_type rhs) {
					return lhs -= rhs;
				}

			private:

				// TODO <your data>

			private:

				bool valid() const {
					// <your code>
					return true;
				}

			public:

				/**
				 * <your documentation>
				 */
				iterator(/* <your arguments> */) {
					// <your code>
					// assert(valid());
				}

				/**
				 * <your documentation>
				 */
				reference operator *() const {
					// <your code>
					// dummy is just to be able to compile the skeleton, remove it
					static value_type dummy;
					return dummy;
				}

				/**
				 * <your documentation>
				 */
				pointer operator ->() const {
					return &**this;
				}

				/**
				 * <your documentation>
				 */
				iterator& operator ++() {
					// <your code>
					assert(valid());
					return *this;
				}

				/**
				 * <your documentation>
				 */
				iterator operator ++(int) {
					iterator x = *this;
					++(*this);
					assert(valid());
					return x;
				}

				/**
				 * <your documentation>
				 */
				iterator& operator --() {
					// <your code>
					assert(valid());
					return *this;
				}

				/**
				 * <your documentation>
				 */
				iterator operator --(int) {
					iterator x = *this;
					--(*this);
					assert(valid());
					return x;
				}

				/**
				 * <your documentation>
				 */
				iterator& operator +=(difference_type d) {
					// <your code>
					assert(valid());
					return *this;
				}

				/**
				 * <your documentation>
				 */
				iterator& operator -=(difference_type d) {
					// <your code>
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
				 * <your documentation>
				 */
				friend bool operator ==(const const_iterator& lhs, const const_iterator& rhs) {
					// <your code>
					return true;
				}

				/**
				 * <your documentation>
				 */
				friend bool operator !=(const const_iterator& lhs, const const_iterator& rhs) {
					return !(lhs == rhs);
				}

				/**
				 * <your documentation>
				 */
				friend const_iterator operator +(const_iterator lhs, difference_type rhs) {
					return lhs += rhs;
				}

				/**
				 * <your documentation>
				 */
				friend const_iterator operator -(const_iterator lhs, difference_type rhs) {
					return lhs -= rhs;
				}

			private:
				// TODO <your data>

			private:
				bool valid() const {
					// <your code>
					return true;
				}

			public:
				/**
				 * <your documentation>
				 */
				const_iterator(/* <your arguments> */) {
					// <your code>
					assert(valid());
				}

                /**
                 * <your documentation>
                 */
                const_iterator(iterator) {
                    // <your code>
                    assert(valid());
                }

				// Default copy, destructor, and copy assignment.
				// const_iterator (const const_iterator&);
				// ~const_iterator ();
				// const_iterator& operator = (const const_iterator&);

				/**
				 * <your documentation>
				 */
				reference operator *() const {
					// <your code>
					// dummy is just to be able to compile the skeleton, remove it
					static value_type dummy;
					return dummy;
				}

				/**
				 * <your documentation>
				 */
				pointer operator ->() const {
					return &**this;
				}

				/**
				 * <your documentation>
				 */
				const_iterator& operator ++() {
					// <your code>
					assert(valid());
					return *this;
				}

				/**
				 * <your documentation>
				 */
				const_iterator operator ++(int) {
					const_iterator x = *this;
					++(*this);
					assert(valid());
					return x;
				}

				/**
				 * <your documentation>
				 */
				const_iterator& operator --() {
					// <your code>
					assert(valid());
					return *this;
				}

				/**
				 * <your documentation>
				 */
				const_iterator operator --(int) {
					const_iterator x = *this;
					--(*this);
					assert(valid());
					return x;
				}

				/**
				 * <your documentation>
				 */
				const_iterator& operator +=(difference_type) {
					// <your code>
					assert(valid());
					return *this;
				}

				/**
				 * <your documentation>
				 */
				const_iterator& operator -=(difference_type) {
					// <your code>
					assert(valid());
					return *this;
				}
		};

	public:
		/**
		 * <your documentation>
		 */
		explicit MyDeque(const allocator_type& a = allocator_type()) :
				_size(0),
				_outer_array_size(3),
				_allocator(a),
				_pointer_allocator() {
			// <your code>
			assert(valid());
		}

		/**
		 * <your documentation>
		 */
		explicit MyDeque(size_type s, const_reference v = value_type(), const allocator_type& a = allocator_type()) :
				_size(s),
				_outer_array_size(s >> LOG_INNER_SIZE),
				_allocator(a),
				_pointer_allocator() {
			// <your code>
			assert(valid());
		}

		/**
		 * <your documentation>
		 */
		MyDeque(const MyDeque& that) :
				_size(that._size),
				_outer_array_size(that._outer_array_size),
				_allocator(that._allocator),
				_pointer_allocator(that._pointer_allocator) {
			// <your code>
			assert(valid());
		}

		/**
		 * <your documentation>
		 */
		~MyDeque() {
			// <your code>
			assert(valid());
		}

		/**
		 * <your documentation>
		 */
		MyDeque& operator =(const MyDeque& rhs) {
			// <your code>
			assert(valid());
			return *this;
		}

		/**
		 * <your documentation>
		 */
		reference operator [](size_type index) {
			// <your code>
			// dummy is just to be able to compile the skeleton, remove it
			static value_type dummy;
			return dummy;
		}

		/**
		 * <your documentation>
		 */
		const_reference operator [](size_type index) const {
			return const_cast<MyDeque*>(this)->operator[](index);
		}

		/**
		 * <your documentation>
		 */
		reference at(size_type index) {
			// <your code>
			// dummy is just to be able to compile the skeleton, remove it
			static value_type dummy;
			return dummy;
		}

		/**
		 * <your documentation>
		 */
		const_reference at(size_type index) const {
			return const_cast<MyDeque*>(this)->at(index);
		}

		/**
		 * <your documentation>
		 */
		reference back() {
			// <your code>
			// dummy is just to be able to compile the skeleton, remove it
			static value_type dummy;
			return dummy;
		}

		/**
		 * <your documentation>
		 */
		const_reference back() const {
			return const_cast<MyDeque*>(this)->back();
		}

		/**
		 * <your documentation>
		 */
		iterator begin() {
			// <your code>
			return iterator(/* <your arguments> */);
		}

		/**
		 * <your documentation>
		 */
		const_iterator begin() const {
			// <your code>
			return const_iterator(/* <your arguments> */);
		}

		/**
		 * <your documentation>
		 */
		void clear() {
			// <your code>
			assert(valid());
		}

		/**
		 * <your documentation>
		 */
		bool empty() const {
			return !size();
		}

		/**
		 * <your documentation>
		 */
		iterator end() {
			// <your code>
			return iterator(/* <your arguments> */);
		}

		/**
		 * <your documentation>
		 */
		const_iterator end() const {
			// <your code>
			return const_iterator(/* <your arguments> */);
		}

		/**
		 * <your documentation>
		 */
		iterator erase(iterator) {
			--_size;
			// <your code>
			assert(valid());
			return iterator();
		}

		/**
		 * <your documentation>
		 */
		reference front() {
			// <your code>
			// dummy is just to be able to compile the skeleton, remove it
			static value_type dummy;
			return dummy;
		}

		/**
		 * <your documentation>
		 */
		const_reference front() const {
			return const_cast<MyDeque*>(this)->front();
		}

		/**
		 * <your documentation>
		 */
		iterator insert(iterator, const_reference) {
			++_size;
			// <your code>
			// assert(valid());
			return iterator();
		}

		/**
		 * <your documentation>
		 */
		void pop_back() {
			--_size;
			// <your code>
			assert(valid());
		}

		/**
		 * <your documentation>
		 */
		void pop_front() {
			--_size;
			// <your code>
			assert(valid());
		}

		/**
		 * <your documentation>
		 */
		void push_back(const_reference) {
			++_size;
			// <your code>
			assert(valid());
		}

		/**
		 * <your documentation>
		 */
		void push_front(const_reference) {
			++_size;
			// <your code>
			assert(valid());
		}

		/**
		 * <your documentation>
		 */
		void resize(size_type s, const_reference v = value_type()) {
			// <your code>
			mySize = s;
			assert(valid());
		}

		/**
		 * <your documentation>
		 */
		size_type size() const {
			return _size;
		}

		/**
		 * <your documentation>
		 */
		void swap(MyDeque&) {
			// <your code>
			assert(valid());
		}
};

#endif // Deque_h
