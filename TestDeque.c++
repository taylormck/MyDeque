/*
 * TestDeque
 *
 * To compile this, use the command
 * g++ -pedantic -std=c++0x -Wall TestDeque.c++ -o TestDeque -lgtest -lgtest_main -lpthread
 *
 * Then it can run with
 * TestDeque
 *
 * It will work on any machine with gtest and the precompiled libraries installed
 */

#include <algorithm> // equal
#include <cstring>   // strcmp
#include <deque>     // deque
#include <sstream>   // ostringstream
#include <stdexcept> // invalid_argument
#include <string>    // ==

// Stuff in deque.h so they don't get compile errors when we use the defines
// to make all members of deque public
#include <cassert>
#include <iterator>
#include <memory>
#include <utility>

#include "gtest/gtest.h" // Google Test framework

#define class struct
#define protected public
#define private public

#include "Deque.h"


// Not testing the code we didn't write
// destroy, unitialized_copy, unitialized_fill

// --- Deque Interface tests ---
// These are tests that both deques should pass

template <typename C>
class DequeTest : public testing::Test {
	protected:
		typedef typename C::value_type T;
		std::allocator<T> a;
		C x, y, allocator_constructed, size_construced, copy_constructed;

		// This needs to be large enough that the data will span past
		// a single array
		const size_t s;

		DequeTest() :
			x(), y(),
			allocator_constructed(a),
			size_construced(10, 5),
			copy_constructed(size_construced),
			s(10000)
		{}

		void SetSame() {
			x = C (10, 5);
			y = C (10, 5);
		}

		void SetDifferent() {
			SetSame();
			y[5] = 3;
		}

		void SetLessContent() {
			x = C (10, 4);
			y = C (10, 5);
		}

		void SetLessSize() {
			x = C(5, 5);
			y = C(10, 5);
		}

		void SetLarge() {
			x = C (s, 5);
			y = C (s, 5);
		}
};

typedef testing::Types<std::deque<int>, MyDeque<int> > MyDeques;
TYPED_TEST_CASE(DequeTest, MyDeques);

// --- operator == ---

TYPED_TEST(DequeTest, ContentEqualsOnEmpty) {
	EXPECT_EQ(this->x, this->y);
}

TYPED_TEST(DequeTest, ContentEqualsSmall) {
	this->SetSame();
	EXPECT_EQ(this->x, this->y);
}

TYPED_TEST(DequeTest, ContentEqualsRough) {
	this->SetSame();

	for (int i = 0; i < 100; ++i)
		this->x.push_back(i);
	for (int i = 0; i < 100; ++i)
		this->x.pop_back();

	EXPECT_EQ(this->x, this->y);
}

TYPED_TEST(DequeTest, ContentEqualsLarge) {
	this->SetLarge();
	EXPECT_EQ(this->x, this->y);
}

TYPED_TEST(DequeTest, ContentNotEqual) {
	this->SetDifferent();
	EXPECT_NE(this->x, this->y);
}

TYPED_TEST(DequeTest, SizeNotEqualSize) {
	this->SetSame();
	this->x.push_back(0);
	EXPECT_NE(this->x, this->y);
}

TYPED_TEST(DequeTest, ContentAndSizeNotEqual) {
	this->SetSame();
	this->x.push_back(0);
	EXPECT_NE(this->x, this->y);
}

TYPED_TEST(DequeTest, ContentNotEqualsLarge) {
	this->SetLarge();
	this->y[this->s - 1] = 0;
	EXPECT_NE(this->x, this->y);
}

// --- operator < ---

TYPED_TEST(DequeTest, LessThan) {
	this->SetLessContent();
	EXPECT_LT(this->x, this->y);
}

TYPED_TEST(DequeTest, LessThanSize) {
	this->SetLessSize();
	EXPECT_LE(this->x, this->y);
}

TYPED_TEST(DequeTest, LessThanLarge) {
	this->SetLarge();
	this->x[0] = 0;
	EXPECT_LE(this->x, this->y);
}

TYPED_TEST(DequeTest, LessThanEqualEmpty) {
	EXPECT_LE(this->x, this->y);
}

TYPED_TEST(DequeTest, LessThanEqual) {
	this->SetLessContent();
	EXPECT_LE(this->x, this->y);
}

TYPED_TEST(DequeTest, LessThanEqualSelf) {
	this->SetLessContent();
	EXPECT_LE(this->x, this->x);
}

TYPED_TEST(DequeTest, LessThanEqualSize) {
	this->SetLessSize();
	EXPECT_LE(this->x, this->y);
}

TYPED_TEST(DequeTest, LessThanEqualLarge) {
	this->SetLarge();
	EXPECT_LE(this->x, this->y);
	this->x[0] = 0;
	EXPECT_LE(this->x, this->y);
}

TYPED_TEST(DequeTest, GreaterThanEqualEmpty) {
	EXPECT_GE(this->y, this->x);
}

TYPED_TEST(DequeTest, GreaterThan) {
	this->SetLessContent();
	EXPECT_GT(this->y, this->x);
}

TYPED_TEST(DequeTest, GreaterThanSize) {
	this->SetLessSize();
	EXPECT_GT(this->y, this->x);
}

TYPED_TEST(DequeTest, GreaterThanLarge) {
	this->SetLarge();
	this->x[0] = 0;
	EXPECT_GE(this->y, this->x);
}

TYPED_TEST(DequeTest, GreaterThanEqual) {
	this->SetLessContent();
	EXPECT_GE(this->y, this->x);
}

TYPED_TEST(DequeTest, GreaterThanEqualSelf) {
	this->SetLessContent();
	EXPECT_GE(this->x, this->x);
}

TYPED_TEST(DequeTest, GreaterThanEqualSize) {
	this->SetLessSize();
	EXPECT_GE(this->y, this->x);
}

TYPED_TEST(DequeTest, GreaterThanEqualLarge) {
	this->SetLarge();
	EXPECT_GE(this->y, this->x);
	this->x[0] = 0;
	EXPECT_GE(this->y, this->x);
}

// --- Constructors ---
// TODO fix these tests to actually call the constructor
// use TestFixture::C

TYPED_TEST(DequeTest, AllocatorConstructor) {
	EXPECT_EQ(0, this->x.size());
	EXPECT_EQ(0, this->y.size());
}

TYPED_TEST(DequeTest, SizedConstructor) {
	ASSERT_EQ(10, this->size_construced.size());
	for (int i = 0; i < 10; ++i)
		EXPECT_EQ(5, this->size_construced[i]);
}

TYPED_TEST(DequeTest, CopyConstructor) {
	ASSERT_EQ(this->size_construced.size(), this->copy_constructed.size());

	for(unsigned int i = 0; i < this->size_construced.size(); ++i)
		EXPECT_EQ(this->size_construced[i], this->copy_constructed[i]);
}

// --- Copy Assignment ---

TYPED_TEST(DequeTest, CopyAssignment) {
	this->SetDifferent();
	EXPECT_EQ(3, this->y[5]);

	this->y = this->x;

	ASSERT_EQ(this->x.size(), this->y.size());
	for (unsigned int i = 0; i < this->x.size(); ++i)
		EXPECT_EQ(this->x[i], this->y[i]);
}

// --- operator [] ---

TYPED_TEST(DequeTest, IndexZero) {
	ASSERT_EQ(0, this->x.size());
	this->x.push_back(9);
	EXPECT_EQ(9, this->x[0]);
}

TYPED_TEST(DequeTest, IndexSizeMinusOne) {
	this->SetSame();
	ASSERT_EQ(10, this->x.size());
	this->x.push_back(9);
	EXPECT_EQ(9, this->x[this->x.size() - 1]);
}

// --- at ---

TYPED_TEST(DequeTest, AtZero) {
	ASSERT_EQ(0, this->x.size());
	this->x.push_back(9);
	EXPECT_EQ(9, this->x.at(0));
}

TYPED_TEST(DequeTest, AtSizeMinusOne) {
	this->SetSame();
	ASSERT_EQ(10, this->x.size());
	this->x.push_back(9);
	EXPECT_EQ(9, this->x.at(this->x.size() - 1));
}

TYPED_TEST(DequeTest, AtSizeMinusOneWhenSizeIsLarge) {
	this->SetLarge();
	ASSERT_EQ(this->s, this->x.size());
	this->x.push_back(9);
	EXPECT_EQ(9, this->x.at(this->x.size() - 1));
}

// --- back ---

TYPED_TEST(DequeTest, BackWhenSizeIsOne) {
	ASSERT_EQ(0, this->x.size());
	this->x.push_back(9);
	EXPECT_EQ(9, this->x.back());
}

TYPED_TEST(DequeTest, BackWhenSizeIsSmall) {
	this->SetSame();
	ASSERT_EQ(10, this->x.size());
	this->x.push_back(9);
	EXPECT_EQ(9, this->x.back());
}

TYPED_TEST(DequeTest, BackWhenSizeIsLarge) {
	this->SetLarge();
	ASSERT_EQ(this->s, this->x.size());
	this->x.push_back(9);
	EXPECT_EQ(9, this->x.back());
}

// --- clear ---

TYPED_TEST(DequeTest, ClearEmpty) {
	ASSERT_EQ(0, this->x.size());
	this->x.clear();
	ASSERT_EQ(0, this->x.size());
}

TYPED_TEST(DequeTest, ClearSmall) {
	this->SetSame();
	ASSERT_EQ(10, this->x.size());
	this->x.clear();
	ASSERT_EQ(0, this->x.size());
}

TYPED_TEST(DequeTest, ClearLarge) {
	this->SetLarge();
	ASSERT_EQ(this->s, this->x.size());
	this->x.clear();
	ASSERT_EQ(0, this->x.size());
}

// --- empty ---

TYPED_TEST(DequeTest, EmptyEmpty) {
	ASSERT_TRUE(this->x.empty());
}

TYPED_TEST(DequeTest, EmptySmall) {
	this->SetSame();
	ASSERT_FALSE(this->x.empty());
}

TYPED_TEST(DequeTest, EmptyLarge) {
	this->SetLarge();
	ASSERT_FALSE(this->x.empty());
}

// --- front ---

TYPED_TEST(DequeTest, FrontWhenSizeIsOne) {
	ASSERT_EQ(0, this->x.size());
	this->x.push_front(9);
	ASSERT_EQ(9, this->x.front());
}

TYPED_TEST(DequeTest, FrontWhenSizeIsSmall) {
	this->SetSame();
	ASSERT_EQ(10, this->x.size());
	this->x.push_front(9);
	ASSERT_EQ(9, this->x.front());
}

TYPED_TEST(DequeTest, FrontWhenSizeIsLarge) {
	this->SetLarge();
	ASSERT_EQ(this->s, this->x.size());
	this->x.push_front(9);
	ASSERT_EQ(9, this->x.front());
}

// --- pop_back ---

TYPED_TEST(DequeTest, PopBackWhenSizeIsOne) {
	ASSERT_EQ(0, this->x.size());
	this->x.push_back(0);
	ASSERT_EQ(1, this->x.size());
	this->x.pop_back();
	ASSERT_EQ(0, this->x.size());
}

TYPED_TEST(DequeTest, PopBackWhenSizeIsSmall) {
	this->SetSame();
	EXPECT_EQ(10, this->x.size());
	this->x.front() = 0;
	this->x.back() = 0;

	this->x.pop_back();
	EXPECT_EQ(9, this->x.size());

	EXPECT_EQ(0, this->x.front());
	EXPECT_NE(0, this->x.back());
}

TYPED_TEST(DequeTest, PopBackWhenSizeIsLarge) {
	this->SetLarge();
	EXPECT_EQ(this->s, this->x.size());
	this->x.front() = 0;
	this->x.back() = 0;

	this->x.pop_back();
	EXPECT_EQ(this->s - 1, this->x.size());

	EXPECT_EQ(0, this->x.front());
	EXPECT_NE(0, this->x.back());
}

// --- pop_front ---

TYPED_TEST(DequeTest, PopFrontWhenSizeIsOne) {
	ASSERT_EQ(0, this->x.size());
	this->x.push_front(0);
	ASSERT_EQ(1, this->x.size());
	this->x.pop_front();
	ASSERT_EQ(0, this->x.size());
}

TYPED_TEST(DequeTest, PopFrontWhenSizeIsSmall) {
	this->SetSame();
	EXPECT_EQ(10, this->x.size());
	this->x.front() = 0;
	this->x.back() = 0;

	this->x.pop_front();
	EXPECT_EQ(9, this->x.size());

	EXPECT_NE(0, this->x.front());
	EXPECT_EQ(0, this->x.back());
}

TYPED_TEST(DequeTest, PopFrontWhenSizeIsLarge) {
	this->SetLarge();
	EXPECT_EQ(this->s, this->x.size());
	this->x.front() = 0;
	this->x.back() = 0;

	this->x.pop_front();
	EXPECT_EQ(this->s - 1, this->x.size());

	EXPECT_NE(0, this->x.front());
	EXPECT_EQ(0, this->x.back());
}


// --- push_back ---

TYPED_TEST(DequeTest, PushBackSizeIsZero) {
	this->x.push_back(1);
	EXPECT_EQ(1, this->x.back());
}

TYPED_TEST(DequeTest, PushBackSizeIsSmall) {
	this->SetSame();
	this->x.push_back(1);
	EXPECT_EQ(1, this->x.back());
}

TYPED_TEST(DequeTest, PushBackSizeIsLarge) {
	this->SetLarge();
	this->x.push_back(1);
	EXPECT_EQ(1, this->x.back());
}


// --- push_front ---

TYPED_TEST(DequeTest, PushFrontSizeIsZero) {
	this->x.push_front(1);
	EXPECT_EQ(1, this->x.front());
}

TYPED_TEST(DequeTest, PushFrontSizeIsSmall) {
	this->SetSame();
	this->x.push_front(1);
	EXPECT_EQ(1, this->x.front());
}

TYPED_TEST(DequeTest, PushFrontSizeIsLarge) {
	this->SetLarge();
	this->x.push_front(1);
	EXPECT_EQ(1, this->x.front());
}

// --- resize ---

TYPED_TEST(DequeTest, ResizeGrow) {
	this->SetSame();
	ASSERT_EQ(10, this->x.size());
	ASSERT_EQ(5, this->x.back());
	this->x.resize(11, 9);
	ASSERT_EQ(11, this->x.size());
	ASSERT_EQ(9, this->x.back());
}

TYPED_TEST(DequeTest, ResizeShrink) {
	this->SetSame();
	ASSERT_EQ(10, this->x.size());
	ASSERT_EQ(5, this->x.back());
	this->x.resize(4, 9);
	ASSERT_EQ(4, this->x.size());
	ASSERT_EQ(5, this->x.back());
}

TYPED_TEST(DequeTest, ResizeGrowLarge) {
	this->SetLarge();
	ASSERT_EQ(this->s, this->x.size());
	ASSERT_EQ(5, this->x.back());
	this->x.resize(this->s * 2, 9);
	ASSERT_EQ(this->s * 2, this->x.size());
	ASSERT_EQ(9, this->x.back());
}

TYPED_TEST(DequeTest, ResizeShrinkLarge) {
	this->SetLarge();
	ASSERT_EQ(this->s, this->x.size());
	ASSERT_EQ(5, this->x.back());
	this->x.resize(this->s / 2, 9);
	ASSERT_EQ(this->s / 2, this->x.size());
	ASSERT_EQ(5, this->x.back());
}

// --- size ---

TYPED_TEST(DequeTest, SizeIsZero) {
	ASSERT_EQ(0, this->x.size());
}

TYPED_TEST(DequeTest, SizeIsSmall) {
	this->SetSame();
	EXPECT_EQ(10, this->x.size());
}

TYPED_TEST(DequeTest, SizeIsLarge) {
	this->SetLarge();
	EXPECT_EQ(this->s, this->x.size());
}


// --- swap ---

TYPED_TEST(DequeTest, SwapEmpty) {
	EXPECT_EQ(0, this->x.size());
	EXPECT_EQ(0, this->y.size());
	std::swap(this->x, this->y);  // This will call x.swap
	EXPECT_EQ(0, this->x.size());
	EXPECT_EQ(0, this->y.size());
}

TYPED_TEST(DequeTest, SwapSmall) {
	this->SetDifferent();
	EXPECT_EQ(10, this->x.size());
	EXPECT_EQ(10, this->y.size());
	EXPECT_EQ(5, this->x[5]);
	EXPECT_EQ(3, this->y[5]);

	std::swap(this->x, this->y);  // This will call x.swap

	EXPECT_EQ(10, this->x.size());
	EXPECT_EQ(10, this->y.size());
	EXPECT_EQ(3, this->x[5]);
	EXPECT_EQ(5, this->y[5]);

	this->y.push_back(5);
	EXPECT_EQ(11, this->y.size());

	std::swap(this->x, this->y);

	EXPECT_EQ(11, this->x.size());
	EXPECT_EQ(10, this->y.size());
	EXPECT_EQ(5, this->x[5]);
	EXPECT_EQ(3, this->y[5]);
}

TYPED_TEST(DequeTest, SwapLarge) {
	this->SetLarge();
	this->y[5] = 3;

	EXPECT_EQ(this->s, this->x.size());
	EXPECT_EQ(this->s, this->y.size());
	EXPECT_EQ(5, this->x[5]);
	EXPECT_EQ(3, this->y[5]);

	std::swap(this->x, this->y);  // This will call x.swap

	EXPECT_EQ(this->s, this->x.size());
	EXPECT_EQ(this->s, this->y.size());
	EXPECT_EQ(3, this->x[5]);
	EXPECT_EQ(5, this->y[5]);

	this->y.push_back(5);
	EXPECT_EQ(this->s + 1, this->y.size());

	std::swap(this->x, this->y);

	EXPECT_EQ(this->s + 1, this->x.size());
	EXPECT_EQ(this->s, this->y.size());
	EXPECT_EQ(5, this->x[5]);
	EXPECT_EQ(3, this->y[5]);
}

// --- Iterator Interface Tests ---
// Test the iterators and related methods of both classes
// Methods that use iterators are also tested here
// CAUTION: You can't copmare iterators using *_EQ

template<typename C>
class IteratorTest : public testing::Test {
	protected:
		typedef typename C::value_type value_type;
		typedef typename C::iterator iterator;
		typedef typename C::const_iterator const_iterator;
		C x, y;
		iterator i, j;
		value_type m, n;

		void SetUpBegin() {
			x.push_back(0);
			x.push_back(1);
			x.push_back(2);
			y = x;

			i = x.begin();
			j = y.begin();
		}

		void SetUpEnd() {
			x.push_back(0);
			x.push_back(1);
			x.push_back(2);
			y = x;

			i = x.end();
			j = y.end();
		}
};

TYPED_TEST_CASE(IteratorTest, MyDeques);

// --- iterator constructor ---
// TODO

// --- iterator operator == ---

TYPED_TEST(IteratorTest, IteratorEqualsSelf) {
	this->SetUpBegin();
	ASSERT_TRUE(this->i == this->i);
}

TYPED_TEST(IteratorTest, IteratorNotEqualBeginAndEnd) {
	this->SetUpBegin();
	this->j = this->x.end();
	ASSERT_FALSE(this->i == this->j);
}

TYPED_TEST(IteratorTest, IteratorEqualsSelfEnd) {
	this->SetUpEnd();
	ASSERT_TRUE(this->i == this->i);
}

TYPED_TEST(IteratorTest, IteratorEqualsEnd) {
	this->SetUpEnd();
	ASSERT_FALSE(this->i == this->j);
}

TYPED_TEST(IteratorTest, IteratorNotEquals) {
	this->SetUpBegin();
	ASSERT_FALSE(this->i == this->j);
}

// --- operator * ---
// TODO
TYPED_TEST(IteratorTest, IteratorDereferenceI) {
	this->SetUpBegin();
	ASSERT_EQ(0, *(this->i));
}

TYPED_TEST(IteratorTest, IteratorDereferenceJ) {
	this->SetUpBegin();
	ASSERT_EQ(0, *(this->j));
}

// --- operator += ---

TYPED_TEST(IteratorTest, IteratorPlusEqualBeginToEnd) {
	this->SetUpBegin();
	ASSERT_TRUE(this->x.begin() == this->i);
	ASSERT_TRUE(this->x.end() == (this->i += 3));
}

TYPED_TEST(IteratorTest, IteratorPlusEqualBeginStepped) {
	this->SetUpBegin();
	ASSERT_TRUE(this->i == this->x.begin());

	for (unsigned int count = 0; count < this->x.size(); ++count)
		ASSERT_FALSE(this->x.begin() == (this->i += 1));

	ASSERT_TRUE(this->x.end() == this->i);
}

// --- operator -= ---
// TODO
TYPED_TEST(IteratorTest, IteratorMinusEqual) {
	this->SetUpEnd();
	ASSERT_TRUE(this->x.end() == this->i);
	ASSERT_TRUE(this->x.begin() == (this->i -= 3));
}

TYPED_TEST(IteratorTest, IteratorPlusEqualStepped) {
	this->SetUpEnd();
	ASSERT_TRUE(this->i == this->x.end());

	for (unsigned int count = 0; count < this->x.size() - 1; ++count)
		ASSERT_FALSE(this->x.begin() == (this->i -= 1));

	ASSERT_TRUE(this->x.begin() == (this->i -= 1));
}


// --- operator ++ pre ---

TYPED_TEST(IteratorTest, IteratorPreIncrement) {
	this->SetUpBegin();
	ASSERT_TRUE((this->x.begin() + 1) == (++(this->i)));
}

TYPED_TEST(IteratorTest, IteratorPreIncrementNoTemp) {
	this->SetUpBegin();
	typename TestFixture::iterator ti = ++(this->i);
	ASSERT_TRUE(ti == (this->i));
}

// --- operator -- pre ---
// TODO
TYPED_TEST(IteratorTest, IteratorPreDecrement) {
	this->SetUpEnd();
	ASSERT_TRUE((this->x.end() - 1) == (--(this->i)));
}

TYPED_TEST(IteratorTest, IteratorPreDecrementNoTemp) {
	this->SetUpEnd();
	typename TestFixture::iterator ti = --(this->i);
	ASSERT_TRUE(ti == (this->i));
}


// --- const_iterator constructor ---
// TODO

// --- const_iterator operator == ---

TYPED_TEST(IteratorTest, ConstIteratorEqualsSelf) {
	this->SetUpBegin();
	typename TestFixture::const_iterator ci = this->i;
	ASSERT_TRUE(ci == ci);
}

TYPED_TEST(IteratorTest, ConstIteratorEqualsSelfEnd) {
	this->SetUpEnd();
	typename TestFixture::const_iterator ci = this->i;
	ASSERT_TRUE(ci == ci);
}

TYPED_TEST(IteratorTest, ConstIteratorNotEquals) {
	this->SetUpBegin();
	typename TestFixture::const_iterator ci = this->i;
	typename TestFixture::const_iterator cj = this->j;
	ASSERT_FALSE(ci == cj);
}

TYPED_TEST(IteratorTest, ConstIteratorNotEqualsEnd) {
	this->SetUpEnd();
	typename TestFixture::const_iterator ci = this->i;
	typename TestFixture::const_iterator cj = this->j;
	ASSERT_FALSE(ci == cj);
}

// --- const_iterator operator * ---
// TODO
TYPED_TEST(IteratorTest, ConstIteratorDereferenceI) {
	this->SetUpBegin();
	typename TestFixture::const_iterator ci = this->i;
	ASSERT_EQ(0, *ci);
}

TYPED_TEST(IteratorTest, ConstIteratorDereferenceJ) {
	this->SetUpBegin();
	typename TestFixture::const_iterator cj = this->j;
	ASSERT_EQ(0, *cj);
}

// --- const_iterator operator += ---

TYPED_TEST(IteratorTest, ConstIteratorPlusEqualBeginToEnd) {
	this->SetUpBegin();
	typename TestFixture::const_iterator ci = this->i;
	ASSERT_TRUE(this->x.begin() == ci);
	ASSERT_TRUE(this->x.end() == (ci += 3));
}

TYPED_TEST(IteratorTest, ConstIteratorPlusEqualBeginStepped) {
	this->SetUpBegin();
	typename TestFixture::const_iterator ci = this->i;
	ASSERT_TRUE(ci == this->x.begin());

	for (unsigned int count = 0; count < this->x.size(); ++count)
		ASSERT_FALSE(this->x.begin() == (ci += 1));

	ASSERT_TRUE(this->x.end() == ci);
}

// --- const_iterator operator -= ---
// TODO
TYPED_TEST(IteratorTest, ConstIteratorMinusEqual) {
	this->SetUpEnd();
	typename TestFixture::const_iterator ci = this->i;
	ASSERT_TRUE(this->x.end() == ci);
	ASSERT_TRUE(this->x.begin() == (ci -= 3));
}

TYPED_TEST(IteratorTest, ConstIteratorMinusEqualStepped) {
	this->SetUpEnd();
	typename TestFixture::const_iterator ci = this->i;
	ASSERT_TRUE(ci == this->x.end());

	for (unsigned int count = 0; count < this->x.size() - 1; ++count)
		ASSERT_FALSE(this->x.begin() == (ci -= 1));

	ASSERT_TRUE(this->x.begin() == (ci -= 1));
}

// --- const_iterartor operator ++ pre ---
// TODO
TYPED_TEST(IteratorTest, ConstIteratorPreIncrement) {
	this->SetUpBegin();
	typename TestFixture::const_iterator ci = this->i;
	ASSERT_TRUE((this->x.begin() + 1) == (++ci));
}

TYPED_TEST(IteratorTest, ConstIteratorPreIncrementNoTemp) {
	this->SetUpBegin();
	typename TestFixture::const_iterator ci = this->i;
	typename TestFixture::const_iterator ti = ++ci;
	ASSERT_TRUE(ti == ci);
}


// --- const_iterator operator -- pre ---
// TODO
TYPED_TEST(IteratorTest, ConstIteratorPreDecrement) {
	this->SetUpEnd();
	typename TestFixture::const_iterator ci = this->i;
	ASSERT_TRUE((this->x.end() - 1) == (--ci));
}

TYPED_TEST(IteratorTest, ConstIteratorPreDecrementNoTemp) {
	this->SetUpEnd();
	typename TestFixture::const_iterator ci = this->i;
	typename TestFixture::const_iterator ti = --ci;
	ASSERT_TRUE(ti == ci);
}


// --- begin ---
// TODO

// --- end ---
// TODO

// --- erase ---
// TODO

// --- insert ---
// TODO

// --- MyDeque Implementation Tests ---
// These are tests tailored to MyDeque

template<typename T>
class MyDequeTest : public testing::Test {
	protected:
		typedef typename MyDeque<T>::iterator iterator;
		MyDeque<T> x, y;
};

typedef testing::Types<int, char, std::string> MyTypes;
TYPED_TEST_CASE(MyDequeTest, MyTypes);

// --- valid ---
// TODO

// --- iterator::valid ---
// TODO

