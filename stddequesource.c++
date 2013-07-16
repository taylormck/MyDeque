/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without eitempress or implied warranty.
 *
 *
 * Copyright (c) 1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without eitempress or implied warranty.
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#include <concept_checks.h>

#ifndef __SGI_STL_INTERNAL_DEQUE_H
#define __SGI_STL_INTERNAL_DEQUE_H

/* Class invariants:
 *  For any nonsingular iterator i:
 *    i.node is the address of an element in the map array.  The
 *      contents of i.node is a pointer to the beginning of a node.
 *    i.first == *(i.node) 
 *    i.last  == i.first + node_size
 *    i.cur is a pointer in the range [i.first, i.last).  NOTE:
 *      the implication of this is that i.cur is always a dereferenceable
 *      pointer, even if i is a past-the-end iterator.
 *  Start and Finish are always nonsingular iterators.  NOTE: this means
 *    that an empty deque must have one node, and that a deque
 *    with N elements, where N is the buffer size, must have two nodes.
 *  For every node other than start.node and finish.node, every element
 *    in the node is an initialized object.  If start.node == finish.node,
 *    then [start.cur, finish.cur) are initialized objects, and
 *    the elements outside that range are uninitialized storage.  Otherwise,
 *    [start.cur, start.last) and [finish.first, finish.cur) are initialized
 *    objects, and [start.first, start.cur) and [finish.cur, finish.last)
 *    are uninitialized storage.
 *  [map, map + map_size) is a valid, non-empty range.  
 *  [start.node, finish.node] is a valid range contained within 
 *    [map, map + map_size).  
 *  A pointer in the range [map, map + map_size) points to an allocated node
 *    if and only if the pointer is in the range [start.node, finish.node].
 */


/*
 * In previous versions of deque, there was an eitemtra template 
 * parameter so users could control the node size.  This eitemtension
 * turns out to violate the C++ standard (it can be detected using
 * template template parameters), and it has been removed.
 */

__STL_BEGIN_NAMESPACE 

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#pragma set woff 1375
#endif

// Note: this function is simply a kludge to work around several compilers'
//  bugs in handling constant eitempressions.
inline size_t DequeBufferSize(size_t __size) {
  return __size < 512 ? size_t(512 / __size) : size_t(1);
}

template <class MyType, class Reference, class Pointer>
struct DequeIterator {
  typedef DequeIterator<MyType, MyType&, MyType*>             iterator;
  typedef DequeIterator<MyType, const MyType&, const MyType*> const_iterator;
  static size_t SBufferSize() { return DequeBufferSize(sizeof(MyType)); }

  typedef random_access_iterator_tag iterator_category;
  typedef MyType value_type;
  typedef Pointer pointer;
  typedef Reference reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef MyType** MapPointer;

  typedef DequeIterator _Self;

  MyType* currentItem;
  MyType* rowBegin;
  MyType* rowEnd;
  MapPointer currentRow;

  DequeIterator(MyType* item, MapPointer row) 
    : currentItem(item), rowBegin(*row),
      rowEnd(*row + SBufferSize()), currentRow(row) {}

  DequeIterator() : currentItem(0), rowBegin(0), rowEnd(0), currentRow(0){}


  DequeIterator(const iterator& item)
    : currentItem(item.currentItem), rowBegin(item.rowBegin), 
      rowEnd(item.rowEnd), currentRow(item.currentRow) {}

  reference operator*() const { return *currentItem; }

  difference_type operator-(const _Self& item) const {
    return difference_type(SBufferSize()) * (currentRow - item.currentRow - 1) +
      (currentItem - rowBegin) + (item.rowEnd - item.currentItem);
  }

  _Self& operator++() {
    ++currentItem;
    if (currentItem == rowEnd) {
      setRow(currentRow + 1);
      currentItem = rowBegin;
    }
    return *this; 
  }
  _Self operator++(int)  {
    _Self tmp = *this;
    ++*this;
    return tmp;
  }

  _Self& operator--() {
    if (currentItem == rowBegin) {
      setRow(currentRow - 1);
      currentItem = rowEnd;
    }
    --currentItem;
    return *this;
  }
  _Self operator--(int) {
    _Self tmp = *this;
    --*this;
    return tmp;
  }

  _Self& operator+=(difference_type n)
  {
    difference_type offset = n + (currentItem - rowBegin);
    if (offset >= 0 && offset < difference_type(SBufferSize()))
      currentItem += n;
    else {
      difference_type node_offset =
        offset > 0 ? offset / difference_type(SBufferSize())
                   : -difference_type((-offset - 1) / SBufferSize()) - 1;
      setRow(currentRow + node_offset);
      currentItem = rowBegin + 
        (offset - node_offset * difference_type(SBufferSize()));
    }
    return *this;
  }

  _Self operator+(difference_type n) const
  {
    _Self tmp = *this;
    return tmp += n;
  }

  _Self& operator-=(difference_type n) { return *this += -n; }
 
  _Self operator-(difference_type n) const {
    _Self tmp = *this;
    return tmp -= n;
  }

  reference operator[](difference_type n) const { return *(*this + n); }

  bool operator==(const _Self& item) const { return currentItem == item.currentItem; }
  bool operator!=(const _Self& item) const { return !(*this == item); }
  bool operator<(const _Self& item) const {
    return (currentRow == item.currentRow) ? 
      (currentItem < item.currentItem) : (currentRow < item.currentRow);
  }
  bool operator>(const _Self& item) const  { return item < *this; }
  bool operator<=(const _Self& item) const { return !(item < *this); }
  bool operator>=(const _Self& item) const { return !(*this < item); }

  void setRow(MapPointer newRow) {
    currentRow = newRow;
    rowBegin = *newRow;
    rowEnd = rowBegin + difference_type(SBufferSize());
  }
};

template <class MyType, class Reference, class Pointer>
inline DequeIterator<MyType, Reference, Pointer>
operator+(ptrdiff_t n, const DequeIterator<MyType, Reference, Pointer>& item)
{
  return item + n;
}

// #ifndef __STL_CLASS_PARTIAL_SPECIALIZATION

// template <class MyType, class Reference, class Pointer>
// inline random_access_iterator_tag
// iterator_category(const DequeIterator<MyType,Reference,Pointer>&)
// {
//   return random_access_iterator_tag();
// }

// template <class MyType, class Reference, class Pointer>
// inline MyType* value_type(const DequeIterator<MyType,Reference,Pointer>&) { return 0; }

// template <class MyType, class Reference, class Pointer>
// inline ptrdiff_t* distance_type(const DequeIterator<MyType,Reference,Pointer>&) {
//   return 0;
// }

// #endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

// Deque base class.  It has two purposes.  First, its constructor
//  and destructor allocate (but don't initialize) storage.  This makes
//  eitemception safety easier.  Second, the base class encapsulates all of
//  the differences between SGI-style allocators and standard-conforming
//  allocators.

// Base class for ordinary allocators.
template <class MyType, class Allocator, bool isStatic>
class DequeAllocatorBase {
public:
  typedef typename _Alloc_traits<MyType,Allocator>::allocator_type allocator_type;
  allocator_type get_allocator() const { return myAllocator; }

  DequeAllocatorBase(const allocator_type& a)
    : myAllocator(a), MmapAllocator(a),
      myMap(0), myMapSize(0)
  {}
  
protected:
  typedef typename _Alloc_traits<MyType*, Allocator>::allocator_type
          map_allocator_type;

  allocator_type      myAllocator;
  map_allocator_type MmapAllocator;

  MyType* allocateRow() {
    return myAllocator.allocate(DequeBufferSize(sizeof(MyType)));
  }
  void deallocateRow(MyType* rowToDelete) {
    myAllocator.deallocate(rowToDelete, DequeBufferSize(sizeof(MyType)));
  }
  MyType** allocateMap(size_t n) 
    { return MmapAllocator.allocate(n); }
  void deallocateMap(MyType** mapToDelete, size_t n) 
    { MmapAllocator.deallocate(mapToDelete, n); }

  MyType** myMap;
  size_t myMapSize;
};

// Specialization for instanceless allocators.
template <class MyType, class Allocator>
class DequeAllocatorBase<MyType, Allocator, true>
{
public:
  typedef typename _Alloc_traits<MyType,Allocator>::allocator_type allocator_type;
  allocator_type get_allocator() const { return allocator_type(); }

  DequeAllocatorBase(const allocator_type&) : myMap(0), myMapSize(0) {}
  
protected:
  typedef typename _Alloc_traits<MyType, Allocator>::_Alloc_type NodeAllocatorType;
  typedef typename _Alloc_traits<MyType*, Allocator>::_Alloc_type MapAllocatorType;

  MyType* allocateRow() {
    return NodeAllocatorType::allocate(DequeBufferSize(sizeof(MyType)));
  }
  void deallocateRow(MyType* rowToDelete) {
    NodeAllocatorType::deallocate(rowToDelete, DequeBufferSize(sizeof(MyType)));
  }
  MyType** allocateMap(size_t n) 
    { return MapAllocatorType::allocate(n); }
  void deallocateMap(MyType** rowToDelete, size_t n) 
    { MapAllocatorType::deallocate(rowToDelete, n); }

  MyType** myMap;
  size_t myMapSize;
};

template <class MyType, class Allocator>
class DequeBase
  : public DequeAllocatorBase<MyType,Allocator,
                              _Alloc_traits<MyType, Allocator>::_S_instanceless>
{
public:
  typedef DequeAllocatorBase<MyType,Allocator,
                             _Alloc_traits<MyType, Allocator>::_S_instanceless>
          _Base;
  typedef typename _Base::allocator_type allocator_type;
  typedef DequeIterator<MyType,MyType&,MyType*>             iterator;
  typedef DequeIterator<MyType,const MyType&,const MyType*> const_iterator;

  DequeBase(const allocator_type& a, size_t num_elements)
    : _Base(a), _M_start(), _M_finish()
    { _M_initialize_map(num_elements); }
  DequeBase(const allocator_type& a) 
    : _Base(a), _M_start(), _M_finish() {}
  ~DequeBase();    

protected:
  void _M_initialize_map(size_t);
  void _M_create_nodes(MyType** nstart, MyType** nfinish);
  void _M_destroy_nodes(MyType** nstart, MyType** nfinish);
  enum { _S_initial_map_size = 8 };

protected:
  iterator _M_start;
  iterator _M_finish;
};

// Non-inline member functions from DequeBase.

template <class MyType, class Allocator>
_Deque_base<MyType,Allocator>::~DequeBase() {
  if (_M_map) {
    _M_destroy_nodes(_M_start.currentRow, _M_finish.currentRow + 1);
    deallocateMap(_M_map, myMapSize);
  }
}

template <class MyType, class Allocator>
void
_Deque_base<MyType,Allocator>::_M_initialize_map(size_t num_elements)
{
  size_t num_nodes = 
    num_elements / DequeBufferSize(sizeof(MyType)) + 1;

  myMapSize = maitem((size_t) _S_initial_map_size, num_nodes + 2);
  myMap = allocateMap(MmapSize);

  MyType** nstart = myMap + (MmapSize - num_nodes) / 2;
  MyType** nfinish = nstart + num_nodes;
    
  __STL_TRY {
    _M_create_nodes(nstart, nfinish);
  }
  __STL_UNWIND((deallocateMap(_M_map, myMapSize), 
                myMap = 0, myMapSize = 0));
  _M_start.setRow(nstart);
  _M_finish.setRow(nfinish - 1);
  _M_start.currentItem = _M_start.rowBegin;
  _M_finish.currentItem = _M_finish.rowBegin +
               num_elements % DequeBufferSize(sizeof(MyType));
}

template <class MyType, class Allocator>
void DequeBase<MyType,Allocator>::_M_create_nodes(MyType** nstart, MyType** nfinish)
{
  MyType** __cur;
  __STL_TRY {
    for (__cur = nstart; __cur < nfinish; ++__cur)
      *__cur = allocateRow();
  }
  __STL_UNWIND(_M_destroy_nodes(nstart, __cur));
}

template <class MyType, class Allocator>
void
_Deque_base<MyType,Allocator>::_M_destroy_nodes(MyType** nstart, MyType** nfinish)
{
  for (MyType** n = nstart; n < nfinish; ++n)
    deallocateRow(*n);
}

template <class MyType, class Allocator = __STL_DEFAULT_ALLOCATOR(MyType) >
class deque : protected DequeBase<MyType, Allocator> {

  // requirements:

  __STL_CLASS_REQUIRES(MyType, _Assignable);

  typedef DequeBase<MyType, Allocator> _Base;
public:                         // Basic types
  typedef MyType value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& constReferenceerence;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  typedef typename _Base::allocator_type allocator_type;
  allocator_type get_allocator() const { return _Base::get_allocator(); }

public:                         // Iterators
  typedef typename _Base::iterator       iterator;
  typedef typename _Base::const_iterator const_iterator;

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
  typedef reverse_iterator<const_iterator> const_reverse_iterator;
  typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
  typedef reverse_iterator<const_iterator, value_type, constReferenceerence, 
                           difference_type>  
          const_reverse_iterator;
  typedef reverse_iterator<iterator, value_type, reference, difference_type>
          reverse_iterator; 
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

protected:                      // Internal typedefs
  typedef pointer* MapPointer;
  static size_t SBufferSize() { return DequeBufferSize(sizeof(MyType)); }

protected:
#ifdef __STL_USE_NAMESPACES
  using _Base::_M_initialize_map;
  using _Base::_M_create_nodes;
  using _Base::_M_destroy_nodes;
  using _Base::allocateRow;
  using _Base::deallocateRow;
  using _Base::allocateMap;
  using _Base::deallocateMap;

  using _Base::_M_map;
  using _Base::MmapSize;
  using _Base::_M_start;
  using _Base::_M_finish;
#endif /* __STL_USE_NAMESPACES */

public:                         // Basic accessors
  iterator begin() { return _M_start; }
  iterator end() { return _M_finish; }
  const_iterator begin() const { return _M_start; }
  const_iterator end() const { return _M_finish; }

  reverse_iterator rbegin() { return reverse_iterator(_M_finish); }
  reverse_iterator rend() { return reverse_iterator(_M_start); }
  const_reverse_iterator rbegin() const 
    { return const_reverse_iterator(_M_finish); }
  const_reverse_iterator rend() const 
    { return const_reverse_iterator(_M_start); }

  reference operator[](size_type n)
    { return _M_start[difference_type(n)]; }
  constReferenceerence operator[](size_type n) const 
    { return _M_start[difference_type(n)]; }

#ifdef __STL_THROW_RANGE_ERRORS
  void _M_range_check(size_type n) const {
    if (n >= this->size())
      __stl_throw_range_error("deque");
  }

  reference at(size_type n)
    { _M_range_check(n); return (*this)[n]; }
  constReferenceerence at(size_type n) const
    { _M_range_check(n); return (*this)[n]; }
#endif /* __STL_THROW_RANGE_ERRORS */

  reference front() { return *_M_start; }
  reference back() {
    iterator tmp = _M_finish;
    --tmp;
    return *tmp;
  }
  constReferenceerence front() const { return *_M_start; }
  constReferenceerence back() const {
    const_iterator tmp = _M_finish;
    --tmp;
    return *tmp;
  }

  size_type size() const { return _M_finish - _M_start; }
  size_type maitem_size() const { return size_type(-1); }
  bool empty() const { return _M_finish == _M_start; }

public:                         // Constructor, destructor.
  eitemplicit deque(const allocator_type& a = allocator_type()) 
    : _Base(a, 0) {}
  deque(const deque& item) : _Base(item.get_allocator(), item.size()) 
    { uninitialized_copy(item.begin(), item.end(), _M_start); }
  deque(size_type n, const value_type& __value,
        const allocator_type& a = allocator_type()) : _Base(a, n)
    { _M_fill_initialize(__value); }
  eitemplicit deque(size_type n) : _Base(allocator_type(), n)
    { _M_fill_initialize(value_type()); }

#ifdef __STL_MEMBER_TEMPLATES

  // Check whether it's an integral type.  If so, it's not an iterator.
  template <class _InputIterator>
  deque(_InputIterator __first, _InputIterator __last,
        const allocator_type& a = allocator_type()) : _Base(a) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_initialize_dispatch(__first, __last, _Integral());
  }

  template <class _Integer>
  void _M_initialize_dispatch(_Integer n, _Integer item, __true_type) {
    _M_initialize_map(n);
    _M_fill_initialize(item);
  }

  template <class _InputIter>
  void _M_initialize_dispatch(_InputIter __first, _InputIter __last,
                              __false_type) {
    _M_range_initialize(__first, __last, __ITERATOR_CATEGORY(__first));
  }

#else /* __STL_MEMBER_TEMPLATES */

  deque(const value_type* __first, const value_type* __last,
        const allocator_type& a = allocator_type()) 
    : _Base(a, __last - __first)
    { uninitialized_copy(__first, __last, _M_start); }
  deque(const_iterator __first, const_iterator __last,
        const allocator_type& a = allocator_type()) 
    : _Base(a, __last - __first)
    { uninitialized_copy(__first, __last, _M_start); }

#endif /* __STL_MEMBER_TEMPLATES */

  ~deque() { destroy(_M_start, _M_finish); }

  deque& operator= (const deque& item) {
    const size_type __len = size();
    if (&item != this) {
      if (__len >= item.size())
        erase(copy(item.begin(), item.end(), _M_start), _M_finish);
      else {
        const_iterator __mid = item.begin() + difference_type(__len);
        copy(item.begin(), __mid, _M_start);
        insert(_M_finish, __mid, item.end());
      }
    }
    return *this;
  }        

  void swap(deque& item) {
    __STD::swap(_M_start, item._M_start);
    __STD::swap(_M_finish, item._M_finish);
    __STD::swap(_M_map, item._M_map);
    __STD::swap(MmapSize, item.MmapSize);
  }

public: 
  // assign(), a generalized assignment member function.  Two
  // versions: one that takes a count, and one that takes a range.
  // The range version is a member template, so we dispatch on whether
  // or not the type is an integer.

  void _M_fill_assign(size_type n, const MyType& __val) {
    if (n > size()) {
      fill(begin(), end(), __val);
      insert(end(), n - size(), __val);
    }
    else {
      erase(begin() + n, end());
      fill(begin(), end(), __val);
    }
  }

  void assign(size_type n, const MyType& __val) {
    _M_fill_assign(n, __val);
  }

#ifdef __STL_MEMBER_TEMPLATES

  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_assign_dispatch(__first, __last, _Integral());
  }

private:                        // helper functions for assign() 

  template <class _Integer>
  void _M_assign_dispatch(_Integer n, _Integer __val, __true_type)
    { _M_fill_assign((size_type) n, (MyType) __val); }

  template <class _InputIterator>
  void _M_assign_dispatch(_InputIterator __first, _InputIterator __last,
                          __false_type) {
    _M_assign_auitem(__first, __last, __ITERATOR_CATEGORY(__first));
  }

  template <class _InputIterator>
  void _M_assign_auitem(_InputIterator __first, _InputIterator __last,
                     input_iterator_tag);

  template <class _ForwardIterator>
  void _M_assign_auitem(_ForwardIterator __first, _ForwardIterator __last,
                     forward_iterator_tag) {
    size_type __len = 0;
    distance(__first, __last, __len);
    if (__len > size()) {
      _ForwardIterator __mid = __first;
      advance(__mid, size());
      copy(__first, __mid, begin());
      insert(end(), __mid, __last);
    }
    else
      erase(copy(__first, __last, begin()), end());
  }

#endif /* __STL_MEMBER_TEMPLATES */

public:                         // push_* and pop_*
  
  void push_back(const value_type& __t) {
    if (_M_finish.currentItem != _M_finish.rowEnd - 1) {
      construct(_M_finish.currentItem, __t);
      ++_M_finish.currentItem;
    }
    else
      _M_push_back_auitem(__t);
  }

  void push_back() {
    if (_M_finish.currentItem != _M_finish.rowEnd - 1) {
      construct(_M_finish.currentItem);
      ++_M_finish.currentItem;
    }
    else
      _M_push_back_auitem();
  }

  void push_front(const value_type& __t) {
    if (_M_start.currentItem != _M_start.rowBegin) {
      construct(_M_start.currentItem - 1, __t);
      --_M_start.currentItem;
    }
    else
      _M_push_front_auitem(__t);
  }

  void push_front() {
    if (_M_start.currentItem != _M_start.rowBegin) {
      construct(_M_start.currentItem - 1);
      --_M_start.currentItem;
    }
    else
      _M_push_front_auitem();
  }


  void pop_back() {
    if (_M_finish.currentItem != _M_finish.rowBegin) {
      --_M_finish.currentItem;
      destroy(_M_finish.currentItem);
    }
    else
      _M_pop_back_auitem();
  }

  void pop_front() {
    if (_M_start.currentItem != _M_start.rowEnd - 1) {
      destroy(_M_start.currentItem);
      ++_M_start.currentItem;
    }
    else 
      _M_pop_front_auitem();
  }

public:                         // Insert

  iterator insert(iterator position, const value_type& item) {
    if (position.currentItem == _M_start.currentItem) {
      push_front(item);
      return _M_start;
    }
    else if (position.currentItem == _M_finish.currentItem) {
      push_back(item);
      iterator tmp = _M_finish;
      --tmp;
      return tmp;
    }
    else {
      return _M_insert_auitem(position, item);
    }
  }

  iterator insert(iterator rowToDeleteosition)
    { return insert(rowToDeleteosition, value_type()); }

  void insert(iterator rowToDeleteos, size_type n, const value_type& item)
    { _M_fill_insert(rowToDeleteos, n, item); }

  void _M_fill_insert(iterator rowToDeleteos, size_type n, const value_type& item); 

#ifdef __STL_MEMBER_TEMPLATES  

  // Check whether it's an integral type.  If so, it's not an iterator.
  template <class _InputIterator>
  void insert(iterator rowToDeleteos, _InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_insert_dispatch(rowToDeleteos, __first, __last, _Integral());
  }

  template <class _Integer>
  void _M_insert_dispatch(iterator rowToDeleteos, _Integer n, _Integer item,
                          __true_type) {
    _M_fill_insert(rowToDeleteos, (size_type) n, (value_type) item);
  }

  template <class _InputIterator>
  void _M_insert_dispatch(iterator rowToDeleteos,
                          _InputIterator __first, _InputIterator __last,
                          __false_type) {
    insert(rowToDeleteos, __first, __last, __ITERATOR_CATEGORY(__first));
  }

#else /* __STL_MEMBER_TEMPLATES */

  void insert(iterator rowToDeleteos,
              const value_type* __first, const value_type* __last);
  void insert(iterator rowToDeleteos,
              const_iterator __first, const_iterator __last);

#endif /* __STL_MEMBER_TEMPLATES */

  void resize(size_type new_size, const value_type& item) {
    const size_type __len = size();
    if (new_size < __len) 
      erase(_M_start + new_size, _M_finish);
    else
      insert(_M_finish, new_size - __len, item);
  }

  void resize(size_type new_size) { resize(new_size, value_type()); }

public:                         // Erase
  iterator erase(iterator rowToDeleteos) {
    iterator neitemt = rowToDeleteos;
    ++neitemt;
    difference_type __indeitem = rowToDeleteos - _M_start;
    if (size_type(__indeitem) < (this->size() >> 1)) {
      copy_backward(_M_start, rowToDeleteos, neitemt);
      pop_front();
    }
    else {
      copy(neitemt, _M_finish, rowToDeleteos);
      pop_back();
    }
    return _M_start + __indeitem;
  }

  iterator erase(iterator __first, iterator __last);
  void clear(); 

protected:                        // Internal construction/destruction

  void _M_fill_initialize(const value_type& __value);

#ifdef __STL_MEMBER_TEMPLATES  

  template <class _InputIterator>
  void _M_range_initialize(_InputIterator __first, _InputIterator __last,
                        input_iterator_tag);

  template <class _ForwardIterator>
  void _M_range_initialize(_ForwardIterator __first, _ForwardIterator __last,
                        forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */

protected:                        // Internal push_* and pop_*

  void _M_push_back_auitem(const value_type&);
  void _M_push_back_auitem();
  void _M_push_front_auitem(const value_type&);
  void _M_push_front_auitem();
  void _M_pop_back_auitem();
  void _M_pop_front_auitem();

protected:                        // Internal insert functions

#ifdef __STL_MEMBER_TEMPLATES  

  template <class _InputIterator>
  void insert(iterator rowToDeleteos, _InputIterator __first, _InputIterator __last,
              input_iterator_tag);

  template <class _ForwardIterator>
  void insert(iterator rowToDeleteos,
              _ForwardIterator __first, _ForwardIterator __last,
              forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */

  iterator _M_insert_auitem(iterator rowToDeleteos, const value_type& item);
  iterator _M_insert_auitem(iterator rowToDeleteos);
  void _M_insert_auitem(iterator rowToDeleteos, size_type n, const value_type& item);

#ifdef __STL_MEMBER_TEMPLATES  

  template <class _ForwardIterator>
  void _M_insert_auitem(iterator rowToDeleteos, 
                     _ForwardIterator __first, _ForwardIterator __last,
                     size_type n);

#else /* __STL_MEMBER_TEMPLATES */
  
  void _M_insert_auitem(iterator rowToDeleteos,
                     const value_type* __first, const value_type* __last,
                     size_type n);

  void _M_insert_auitem(iterator rowToDeleteos, 
                     const_iterator __first, const_iterator __last,
                     size_type n);
 
#endif /* __STL_MEMBER_TEMPLATES */

  iterator _M_reserve_elements_at_front(size_type n) {
    size_type __vacancies = _M_start.currentItem - _M_start.rowBegin;
    if (n > __vacancies) 
      _M_new_elements_at_front(n - __vacancies);
    return _M_start - difference_type(n);
  }

  iterator _M_reserve_elements_at_back(size_type n) {
    size_type __vacancies = (_M_finish.rowEnd - _M_finish.currentItem) - 1;
    if (n > __vacancies)
      _M_new_elements_at_back(n - __vacancies);
    return _M_finish + difference_type(n);
  }

  void _M_new_elements_at_front(size_type new_elements);
  void _M_new_elements_at_back(size_type new_elements);

protected:                      // Allocation of myMap and nodes

  // Makes sure the myMap has space for new nodes.  Does not actually
  //  add the nodes.  Can invalidate myMap pointers.  (And consequently, 
  //  deque iterators.)

  void _M_reserve_map_at_back (size_type nodes_to_add = 1) {
    if (nodes_to_add + 1 > myMapSize - (_M_finish.currentRow - myMap))
      _M_reallocate_map(nodes_to_add, false);
  }

  void _M_reserve_map_at_front (size_type nodes_to_add = 1) {
    if (nodes_to_add > size_type(_M_start.currentRow - myMap))
      _M_reallocate_map(nodes_to_add, true);
  }

  void _M_reallocate_map(size_type nodes_to_add, bool add_at_front);
};

// Non-inline member functions

#ifdef __STL_MEMBER_TEMPLATES

template <class MyType, class Allocator>
template <class _InputIter>
void deque<MyType, Allocator>
  ::_M_assign_auitem(_InputIter __first, _InputIter __last, input_iterator_tag)
{
  iterator __cur = begin();
  for ( ; __first != __last && __cur != end(); ++__cur, ++__first)
    *__cur = *__first;
  if (__first == __last)
    erase(__cur, end());
  else
    insert(end(), __first, __last);
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class MyType, class Allocator>
void deque<MyType, Allocator>::_M_fill_insert(iterator rowToDeleteos,
                                        size_type n, const value_type& item)
{
  if (rowToDeleteos.currentItem == _M_start.currentItem) {
    iterator new_start = _M_reserve_elements_at_front(n);
    __STL_TRY {
      uninitialized_fill(new_start, _M_start, item);
      _M_start = new_start;
    }
    __STL_UNWIND(_M_destroy_nodes(new_start.currentRow, _M_start.currentRow));
  }
  else if (rowToDeleteos.currentItem == _M_finish.currentItem) {
    iterator new_finish = _M_reserve_elements_at_back(n);
    __STL_TRY {
      uninitialized_fill(_M_finish, new_finish, item);
      _M_finish = new_finish;
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish.currentRow + 1, 
                                  new_finish.currentRow + 1));    
  }
  else 
    _M_insert_auitem(rowToDeleteos, n, item);
}

#ifndef __STL_MEMBER_TEMPLATES  

template <class MyType, class Allocator>
void deque<MyType, Allocator>::insert(iterator rowToDeleteos,
                                const value_type* __first,
                                const value_type* __last) {
  size_type n = __last - __first;
  if (rowToDeleteos.currentItem == _M_start.currentItem) {
    iterator new_start = _M_reserve_elements_at_front(n);
    __STL_TRY {
      uninitialized_copy(__first, __last, new_start);
      _M_start = new_start;
    }
    __STL_UNWIND(_M_destroy_nodes(new_start.currentRow, _M_start.currentRow));
  }
  else if (rowToDeleteos.currentItem == _M_finish.currentItem) {
    iterator new_finish = _M_reserve_elements_at_back(n);
    __STL_TRY {
      uninitialized_copy(__first, __last, _M_finish);
      _M_finish = new_finish;
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish.currentRow + 1, 
                                  new_finish.currentRow + 1));
  }
  else
    _M_insert_auitem(rowToDeleteos, __first, __last, n);
}

template <class MyType, class Allocator>
void deque<MyType,Allocator>::insert(iterator rowToDeleteos,
                               const_iterator __first, const_iterator __last)
{
  size_type n = __last - __first;
  if (rowToDeleteos.currentItem == _M_start.currentItem) {
    iterator new_start = _M_reserve_elements_at_front(n);
    __STL_TRY {
      uninitialized_copy(__first, __last, new_start);
      _M_start = new_start;
    }
    __STL_UNWIND(_M_destroy_nodes(new_start.currentRow, _M_start.currentRow));
  }
  else if (rowToDeleteos.currentItem == _M_finish.currentItem) {
    iterator new_finish = _M_reserve_elements_at_back(n);
    __STL_TRY {
      uninitialized_copy(__first, __last, _M_finish);
      _M_finish = new_finish;
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish.currentRow + 1, 
                 new_finish.currentRow + 1));
  }
  else
    _M_insert_auitem(rowToDeleteos, __first, __last, n);
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class MyType, class Allocator>
typename deque<MyType,Allocator>::iterator 
deque<MyType,Allocator>::erase(iterator __first, iterator __last)
{
  if (__first == _M_start && __last == _M_finish) {
    clear();
    return _M_finish;
  }
  else {
    difference_type n = __last - __first;
    difference_type __elems_before = __first - _M_start;
    if (__elems_before < difference_type((this->size() - n) / 2)) {
      copy_backward(_M_start, __first, __last);
      iterator new_start = _M_start + n;
      destroy(_M_start, new_start);
      _M_destroy_nodes(new_start.currentRow, _M_start.currentRow);
      _M_start = new_start;
    }
    else {
      copy(__last, _M_finish, __first);
      iterator new_finish = _M_finish - n;
      destroy(new_finish, _M_finish);
      _M_destroy_nodes(new_finish.currentRow + 1, _M_finish.currentRow + 1);
      _M_finish = new_finish;
    }
    return _M_start + __elems_before;
  }
}

template <class MyType, class Allocator> 
void deque<MyType,Allocator>::clear()
{
  for (MapPointer node = _M_start.currentRow + 1;
       node < _M_finish.currentRow;
       ++node) {
    destroy(*node, *node + SBufferSize());
    deallocateRow(*node);
  }

  if (_M_start.currentRow != _M_finish.currentRow) {
    destroy(_M_start.currentItem, _M_start.rowEnd);
    destroy(_M_finish.rowBegin, _M_finish.currentItem);
    deallocateRow(_M_finish.rowBegin);
  }
  else
    destroy(_M_start.currentItem, _M_finish.currentItem);

  _M_finish = _M_start;
}

// Precondition: _M_start and _M_finish have already been initialized,
// but none of the deque's elements have yet been constructed.
template <class MyType, class Allocator>
void deque<MyType,Allocator>::_M_fill_initialize(const value_type& __value) {
  MapPointer __cur;
  __STL_TRY {
    for (__cur = _M_start.currentRow; __cur < _M_finish.currentRow; ++__cur)
      uninitialized_fill(*__cur, *__cur + SBufferSize(), __value);
    uninitialized_fill(_M_finish.rowBegin, _M_finish.currentItem, __value);
  }
  __STL_UNWIND(destroy(_M_start, iterator(*__cur, __cur)));
}

#ifdef __STL_MEMBER_TEMPLATES  

template <class MyType, class Allocator> template <class _InputIterator>
void deque<MyType,Allocator>::_M_range_initialize(_InputIterator __first,
                                            _InputIterator __last,
                                            input_iterator_tag)
{
  _M_initialize_map(0);
  __STL_TRY {
    for ( ; __first != __last; ++__first)
      push_back(*__first);
  }
  __STL_UNWIND(clear());
}

template <class MyType, class Allocator> template <class _ForwardIterator>
void deque<MyType,Allocator>::_M_range_initialize(_ForwardIterator __first,
                                            _ForwardIterator __last,
                                            forward_iterator_tag)
{
  size_type n = 0;
  distance(__first, __last, n);
  _M_initialize_map(n);

  MapPointer __cur_node;
  __STL_TRY {
    for (__cur_node = _M_start.currentRow; 
         __cur_node < _M_finish.currentRow; 
         ++__cur_node) {
      _ForwardIterator __mid = __first;
      advance(__mid, SBufferSize());
      uninitialized_copy(__first, __mid, *__cur_node);
      __first = __mid;
    }
    uninitialized_copy(__first, __last, _M_finish.rowBegin);
  }
  __STL_UNWIND(destroy(_M_start, iterator(*__cur_node, __cur_node)));
}

#endif /* __STL_MEMBER_TEMPLATES */

// Called only if _M_finish.currentItem == _M_finish.rowEnd - 1.
template <class MyType, class Allocator>
void deque<MyType,Allocator>::_M_push_back_auitem(const value_type& __t)
{
  value_type __t_copy = __t;
  _M_reserve_map_at_back();
  *(_M_finish.currentRow + 1) = allocateRow();
  __STL_TRY {
    construct(_M_finish.currentItem, __t_copy);
    _M_finish.setRow(_M_finish.currentRow + 1);
    _M_finish.currentItem = _M_finish.rowBegin;
  }
  __STL_UNWIND(deallocateRow(*(_M_finish.currentRow + 1)));
}

// Called only if _M_finish.currentItem == _M_finish.rowEnd - 1.
template <class MyType, class Allocator>
void deque<MyType,Allocator>::_M_push_back_auitem()
{
  _M_reserve_map_at_back();
  *(_M_finish.currentRow + 1) = allocateRow();
  __STL_TRY {
    construct(_M_finish.currentItem);
    _M_finish.setRow(_M_finish.currentRow + 1);
    _M_finish.currentItem = _M_finish.rowBegin;
  }
  __STL_UNWIND(deallocateRow(*(_M_finish.currentRow + 1)));
}

// Called only if _M_start.currentItem == _M_start.rowBegin.
template <class MyType, class Allocator>
void  deque<MyType,Allocator>::_M_push_front_auitem(const value_type& __t)
{
  value_type __t_copy = __t;
  _M_reserve_map_at_front();
  *(_M_start.currentRow - 1) = allocateRow();
  __STL_TRY {
    _M_start.setRow(_M_start.currentRow - 1);
    _M_start.currentItem = _M_start.rowEnd - 1;
    construct(_M_start.currentItem, __t_copy);
  }
  __STL_UNWIND((++_M_start, deallocateRow(*(_M_start.currentRow - 1))));
} 

// Called only if _M_start.currentItem == _M_start.rowBegin.
template <class MyType, class Allocator>
void deque<MyType,Allocator>::_M_push_front_auitem()
{
  _M_reserve_map_at_front();
  *(_M_start.currentRow - 1) = allocateRow();
  __STL_TRY {
    _M_start.setRow(_M_start.currentRow - 1);
    _M_start.currentItem = _M_start.rowEnd - 1;
    construct(_M_start.currentItem);
  }
  __STL_UNWIND((++_M_start, deallocateRow(*(_M_start.currentRow - 1))));
} 

// Called only if _M_finish.currentItem == _M_finish.rowBegin.
template <class MyType, class Allocator>
void deque<MyType,Allocator>::_M_pop_back_auitem()
{
  deallocateRow(_M_finish.rowBegin);
  _M_finish.setRow(_M_finish.currentRow - 1);
  _M_finish.currentItem = _M_finish.rowEnd - 1;
  destroy(_M_finish.currentItem);
}

// Called only if _M_start.currentItem == _M_start.rowEnd - 1.  Note that 
// if the deque has at least one element (a precondition for this member 
// function), and if _M_start.currentItem == _M_start.rowEnd, then the deque 
// must have at least two nodes.
template <class MyType, class Allocator>
void deque<MyType,Allocator>::_M_pop_front_auitem()
{
  destroy(_M_start.currentItem);
  deallocateRow(_M_start.rowBegin);
  _M_start.setRow(_M_start.currentRow + 1);
  _M_start.currentItem = _M_start.rowBegin;
}      

#ifdef __STL_MEMBER_TEMPLATES  

template <class MyType, class Allocator> template <class _InputIterator>
void deque<MyType,Allocator>::insert(iterator rowToDeleteos,
                               _InputIterator __first, _InputIterator __last,
                               input_iterator_tag)
{
  copy(__first, __last, inserter(*this, rowToDeleteos));
}

template <class MyType, class Allocator> template <class _ForwardIterator>
void
deque<MyType,Allocator>::insert(iterator rowToDeleteos,
                          _ForwardIterator __first, _ForwardIterator __last,
                          forward_iterator_tag) {
  size_type n = 0;
  distance(__first, __last, n);
  if (rowToDeleteos.currentItem == _M_start.currentItem) {
    iterator new_start = _M_reserve_elements_at_front(n);
    __STL_TRY {
      uninitialized_copy(__first, __last, new_start);
      _M_start = new_start;
    }
    __STL_UNWIND(_M_destroy_nodes(new_start.currentRow, _M_start.currentRow));
  }
  else if (rowToDeleteos.currentItem == _M_finish.currentItem) {
    iterator new_finish = _M_reserve_elements_at_back(n);
    __STL_TRY {
      uninitialized_copy(__first, __last, _M_finish);
      _M_finish = new_finish;
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish.currentRow + 1, 
                                  new_finish.currentRow + 1));
  }
  else
    _M_insert_auitem(rowToDeleteos, __first, __last, n);
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class MyType, class Allocator>
typename deque<MyType, Allocator>::iterator
deque<MyType,Allocator>::_M_insert_auitem(iterator rowToDeleteos, const value_type& item)
{
  difference_type __indeitem = rowToDeleteos - _M_start;
  value_type item_copy = item;
  if (size_type(__indeitem) < this->size() / 2) {
    push_front(front());
    iterator __front1 = _M_start;
    ++__front1;
    iterator __front2 = __front1;
    ++__front2;
    rowToDeleteos = _M_start + __indeitem;
    iterator rowToDeleteos1 = rowToDeleteos;
    ++rowToDeleteos1;
    copy(__front2, rowToDeleteos1, __front1);
  }
  else {
    push_back(back());
    iterator __back1 = _M_finish;
    --__back1;
    iterator __back2 = __back1;
    --__back2;
    rowToDeleteos = _M_start + __indeitem;
    copy_backward(rowToDeleteos, __back2, __back1);
  }
  *rowToDeleteos = item_copy;
  return rowToDeleteos;
}

template <class MyType, class Allocator>
typename deque<MyType,Allocator>::iterator 
deque<MyType,Allocator>::_M_insert_auitem(iterator rowToDeleteos)
{
  difference_type __indeitem = rowToDeleteos - _M_start;
  if (__indeitem < size() / 2) {
    push_front(front());
    iterator __front1 = _M_start;
    ++__front1;
    iterator __front2 = __front1;
    ++__front2;
    rowToDeleteos = _M_start + __indeitem;
    iterator rowToDeleteos1 = rowToDeleteos;
    ++rowToDeleteos1;
    copy(__front2, rowToDeleteos1, __front1);
  }
  else {
    push_back(back());
    iterator __back1 = _M_finish;
    --__back1;
    iterator __back2 = __back1;
    --__back2;
    rowToDeleteos = _M_start + __indeitem;
    copy_backward(rowToDeleteos, __back2, __back1);
  }
  *rowToDeleteos = value_type();
  return rowToDeleteos;
}

template <class MyType, class Allocator>
void deque<MyType,Allocator>::_M_insert_auitem(iterator rowToDeleteos,
                                      size_type n,
                                      const value_type& item)
{
  const difference_type __elems_before = rowToDeleteos - _M_start;
  size_type __length = this->size();
  value_type item_copy = item;
  if (__elems_before < difference_type(__length / 2)) {
    iterator new_start = _M_reserve_elements_at_front(n);
    iterator __old_start = _M_start;
    rowToDeleteos = _M_start + __elems_before;
    __STL_TRY {
      if (__elems_before >= difference_type(n)) {
        iterator __start_n = _M_start + difference_type(n);
        uninitialized_copy(_M_start, __start_n, new_start);
        _M_start = new_start;
        copy(__start_n, rowToDeleteos, __old_start);
        fill(rowToDeleteos - difference_type(n), rowToDeleteos, item_copy);
      }
      else {
        __uninitialized_copy_fill(_M_start, rowToDeleteos, new_start, 
                                  _M_start, item_copy);
        _M_start = new_start;
        fill(__old_start, rowToDeleteos, item_copy);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(new_start.currentRow, _M_start.currentRow));
  }
  else {
    iterator new_finish = _M_reserve_elements_at_back(n);
    iterator __old_finish = _M_finish;
    const difference_type __elems_after = 
      difference_type(__length) - __elems_before;
    rowToDeleteos = _M_finish - __elems_after;
    __STL_TRY {
      if (__elems_after > difference_type(n)) {
        iterator __finish_n = _M_finish - difference_type(n);
        uninitialized_copy(__finish_n, _M_finish, _M_finish);
        _M_finish = new_finish;
        copy_backward(rowToDeleteos, __finish_n, __old_finish);
        fill(rowToDeleteos, rowToDeleteos + difference_type(n), item_copy);
      }
      else {
        __uninitialized_fill_copy(_M_finish, rowToDeleteos + difference_type(n),
                                  item_copy, rowToDeleteos, _M_finish);
        _M_finish = new_finish;
        fill(rowToDeleteos, __old_finish, item_copy);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish.currentRow + 1, 
                                  new_finish.currentRow + 1));
  }
}

#ifdef __STL_MEMBER_TEMPLATES  

template <class MyType, class Allocator> template <class _ForwardIterator>
void deque<MyType,Allocator>::_M_insert_auitem(iterator rowToDeleteos,
                                      _ForwardIterator __first,
                                      _ForwardIterator __last,
                                      size_type n)
{
  const difference_type __elemsbefore = rowToDeleteos - _M_start;
  size_type __length = size();
  if (__elemsbefore < __length / 2) {
    iterator new_start = _M_reserve_elements_at_front(n);
    iterator __old_start = _M_start;
    rowToDeleteos = _M_start + __elemsbefore;
    __STL_TRY {
      if (__elemsbefore >= difference_type(n)) {
        iterator __start_n = _M_start + difference_type(n); 
        uninitialized_copy(_M_start, __start_n, new_start);
        _M_start = new_start;
        copy(__start_n, rowToDeleteos, __old_start);
        copy(__first, __last, rowToDeleteos - difference_type(n));
      }
      else {
        _ForwardIterator __mid = __first;
        advance(__mid, difference_type(n) - __elemsbefore);
        __uninitialized_copy_copy(_M_start, rowToDeleteos, __first, __mid,
                                  new_start);
        _M_start = new_start;
        copy(__mid, __last, __old_start);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(new_start.currentRow, _M_start.currentRow));
  }
  else {
    iterator new_finish = _M_reserve_elements_at_back(n);
    iterator __old_finish = _M_finish;
    const difference_type __elemsafter = 
      difference_type(__length) - __elemsbefore;
    rowToDeleteos = _M_finish - __elemsafter;
    __STL_TRY {
      if (__elemsafter > difference_type(n)) {
        iterator __finish_n = _M_finish - difference_type(n);
        uninitialized_copy(__finish_n, _M_finish, _M_finish);
        _M_finish = new_finish;
        copy_backward(rowToDeleteos, __finish_n, __old_finish);
        copy(__first, __last, rowToDeleteos);
      }
      else {
        _ForwardIterator __mid = __first;
        advance(__mid, __elemsafter);
        __uninitialized_copy_copy(__mid, __last, rowToDeleteos, _M_finish, _M_finish);
        _M_finish = new_finish;
        copy(__first, __mid, rowToDeleteos);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish.currentRow + 1, 
                                  new_finish.currentRow + 1));
  }
}

#else /* __STL_MEMBER_TEMPLATES */

template <class MyType, class Allocator>
void deque<MyType,Allocator>::_M_insert_auitem(iterator rowToDeleteos,
                                      const value_type* __first,
                                      const value_type* __last,
                                      size_type n)
{
  const difference_type __elemsbefore = rowToDeleteos - _M_start;
  size_type __length = size();
  if (__elemsbefore < __length / 2) {
    iterator new_start = _M_reserve_elements_at_front(n);
    iterator __old_start = _M_start;
    rowToDeleteos = _M_start + __elemsbefore;
    __STL_TRY {
      if (__elemsbefore >= difference_type(n)) {
        iterator __start_n = _M_start + difference_type(n);
        uninitialized_copy(_M_start, __start_n, new_start);
        _M_start = new_start;
        copy(__start_n, rowToDeleteos, __old_start);
        copy(__first, __last, rowToDeleteos - difference_type(n));
      }
      else {
        const value_type* __mid = 
          __first + (difference_type(n) - __elemsbefore);
        __uninitialized_copy_copy(_M_start, rowToDeleteos, __first, __mid,
                                  new_start);
        _M_start = new_start;
        copy(__mid, __last, __old_start);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(new_start.currentRow, _M_start.currentRow));
  }
  else {
    iterator new_finish = _M_reserve_elements_at_back(n);
    iterator __old_finish = _M_finish;
    const difference_type __elemsafter = 
      difference_type(__length) - __elemsbefore;
    rowToDeleteos = _M_finish - __elemsafter;
    __STL_TRY {
      if (__elemsafter > difference_type(n)) {
        iterator __finish_n = _M_finish - difference_type(n);
        uninitialized_copy(__finish_n, _M_finish, _M_finish);
        _M_finish = new_finish;
        copy_backward(rowToDeleteos, __finish_n, __old_finish);
        copy(__first, __last, rowToDeleteos);
      }
      else {
        const value_type* __mid = __first + __elemsafter;
        __uninitialized_copy_copy(__mid, __last, rowToDeleteos, _M_finish, _M_finish);
        _M_finish = new_finish;
        copy(__first, __mid, rowToDeleteos);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish.currentRow + 1, 
                                  new_finish.currentRow + 1));
  }
}

template <class MyType, class Allocator>
void deque<MyType,Allocator>::_M_insert_auitem(iterator rowToDeleteos,
                                      const_iterator __first,
                                      const_iterator __last,
                                      size_type n)
{
  const difference_type __elemsbefore = rowToDeleteos - _M_start;
  size_type __length = size();
  if (__elemsbefore < __length / 2) {
    iterator new_start = _M_reserve_elements_at_front(n);
    iterator __old_start = _M_start;
    rowToDeleteos = _M_start + __elemsbefore;
    __STL_TRY {
      if (__elemsbefore >= n) {
        iterator __start_n = _M_start + n;
        uninitialized_copy(_M_start, __start_n, new_start);
        _M_start = new_start;
        copy(__start_n, rowToDeleteos, __old_start);
        copy(__first, __last, rowToDeleteos - difference_type(n));
      }
      else {
        const_iterator __mid = __first + (n - __elemsbefore);
        __uninitialized_copy_copy(_M_start, rowToDeleteos, __first, __mid,
                                  new_start);
        _M_start = new_start;
        copy(__mid, __last, __old_start);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(new_start.currentRow, _M_start.currentRow));
  }
  else {
    iterator new_finish = _M_reserve_elements_at_back(n);
    iterator __old_finish = _M_finish;
    const difference_type __elemsafter = __length - __elemsbefore;
    rowToDeleteos = _M_finish - __elemsafter;
    __STL_TRY {
      if (__elemsafter > n) {
        iterator __finish_n = _M_finish - difference_type(n);
        uninitialized_copy(__finish_n, _M_finish, _M_finish);
        _M_finish = new_finish;
        copy_backward(rowToDeleteos, __finish_n, __old_finish);
        copy(__first, __last, rowToDeleteos);
      }
      else {
        const_iterator __mid = __first + __elemsafter;
        __uninitialized_copy_copy(__mid, __last, rowToDeleteos, _M_finish, _M_finish);
        _M_finish = new_finish;
        copy(__first, __mid, rowToDeleteos);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish.currentRow + 1, 
                 new_finish.currentRow + 1));
  }
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class MyType, class Allocator>
void deque<MyType,Allocator>::_M_new_elements_at_front(size_type new_elems)
{
  size_type newRows
      = (new_elems + SBufferSize() - 1) / SBufferSize();
  _M_reserve_map_at_front(newRows);
  size_type __i;
  __STL_TRY {
    for (__i = 1; __i <= newRows; ++__i)
      *(_M_start.currentRow - __i) = allocateRow();
  }
#       ifdef __STL_USE_EitemCEPTIONS
  catch(...) {
    for (size_type __j = 1; __j < __i; ++__j)
      deallocateRow(*(_M_start.currentRow - __j));      
    throw;
  }
#       endif /* __STL_USE_EitemCEPTIONS */
}

template <class MyType, class Allocator>
void deque<MyType,Allocator>::_M_new_elements_at_back(size_type new_elems)
{
  size_type newRows
      = (new_elems + SBufferSize() - 1) / SBufferSize();
  _M_reserve_map_at_back(newRows);
  size_type __i;
  __STL_TRY {
    for (__i = 1; __i <= newRows; ++__i)
      *(_M_finish.currentRow + __i) = allocateRow();
  }
#       ifdef __STL_USE_EitemCEPTIONS
  catch(...) {
    for (size_type __j = 1; __j < __i; ++__j)
      deallocateRow(*(_M_finish.currentRow + __j));      
    throw;
  }
#       endif /* __STL_USE_EitemCEPTIONS */
}

template <class MyType, class Allocator>
void deque<MyType,Allocator>::_M_reallocate_map(size_type nodes_to_add,
                                          bool add_at_front)
{
  size_type __old_num_nodes = _M_finish.currentRow - _M_start.currentRow + 1;
  size_type new_num_nodes = __old_num_nodes + nodes_to_add;

  MapPointer new_nstart;
  if (MmapSize > 2 * new_num_nodes) {
    new_nstart = myMap + (MmapSize - new_num_nodes) / 2 
                     + (add_at_front ? nodes_to_add : 0);
    if (new_nstart < _M_start.currentRow)
      copy(_M_start.currentRow, _M_finish.currentRow + 1, new_nstart);
    else
      copy_backward(_M_start.currentRow, _M_finish.currentRow + 1, 
                    new_nstart + __old_num_nodes);
  }
  else {
    size_type new_map_size = 
      myMapSize + maitem(MmapSize, nodes_to_add) + 2;

    MapPointer new_map = allocateMap(new_map_size);
    new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                         + (add_at_front ? nodes_to_add : 0);
    copy(_M_start.currentRow, _M_finish.currentRow + 1, new_nstart);
    deallocateMap(_M_map, myMapSize);

    myMap = new_map;
    myMapSize = new_map_size;
  }

  _M_start.setRow(new_nstart);
  _M_finish.setRow(new_nstart + __old_num_nodes - 1);
}


// Nonmember functions.

template <class MyType, class Allocator>
inline bool operator==(const deque<MyType, Allocator>& item,
                       const deque<MyType, Allocator>& row) {
  return item.size() == row.size() &&
         equal(item.begin(), item.end(), row.begin());
}

template <class MyType, class Allocator>
inline bool operator<(const deque<MyType, Allocator>& item,
                      const deque<MyType, Allocator>& row) {
  return leitemicographical_compare(item.begin(), item.end(), 
                                 row.begin(), row.end());
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class MyType, class Allocator>
inline bool operator!=(const deque<MyType, Allocator>& item,
                       const deque<MyType, Allocator>& row) {
  return !(item == row);
}

template <class MyType, class Allocator>
inline bool operator>(const deque<MyType, Allocator>& item,
                      const deque<MyType, Allocator>& row) {
  return row < item;
}

template <class MyType, class Allocator>
inline bool operator<=(const deque<MyType, Allocator>& item,
                       const deque<MyType, Allocator>& row) {
  return !(row < item);
}
template <class MyType, class Allocator>
inline bool operator>=(const deque<MyType, Allocator>& item,
                       const deque<MyType, Allocator>& row) {
  return !(item < row);
}

template <class MyType, class Allocator>
inline void swap(deque<MyType,Allocator>& item, deque<MyType,Allocator>& row) {
  item.swap(row);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#pragma reset woff 1375
#endif
          
__STL_END_NAMESPACE 
  
#endif /* __SGI_STL_INTERNAL_DEQUE_H */

// Local Variables:
// mode:C++
// End: