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

typedef testing::Types<std::deque<int>, MyDeque<int> > MyDeques;
// --- Deque Interface tests ---
// These are tests that both deques should pass

template <typename C>
class DequeTest : public testing::Test {
	protected:
		typedef C container;
		typedef typename C::value_type T;
		typedef typename C::size_type size_type;
		std::allocator<T> a;
		container x, y;

		// This needs to be large enough that the data will span past
		// a single array
		const size_type s;

		DequeTest() :
			s(10000)
		{}

		void SetSame() {
			x = container (10, 5);
			y = container (10, 5);
		}

		void SetDifferent() {
			SetSame();
			y[5] = 3;
		}

		void SetLessContent() {
			x = container (10, 4);
			y = container (10, 5);
			ASSERT_EQ(10, x.size());
			ASSERT_EQ(10, y.size());
		}

		void SetLessSize() {
			x = container (5, 5);
			y = container (10, 5);
			ASSERT_EQ(5, x.size());
			ASSERT_EQ(10, y.size());
		}

		void SetLarge() {
			x = container (s, 5);
			y = container (s, 5);
		}
};

TYPED_TEST_CASE(DequeTest, MyDeques);

// --- Constructors ---

TYPED_TEST(DequeTest, AllocatorConstructor) {
	EXPECT_EQ(0, this->x.size());
	EXPECT_EQ(0, this->y.size());
}

TYPED_TEST(DequeTest, SizedConstructor) {
	typename TestFixture::container v(10, 5);
	ASSERT_EQ(10, v.size());
	for (int i = 0; i < 10; ++i)
		EXPECT_EQ(5, v[i]);
}

TYPED_TEST(DequeTest, CopyConstructor) {
	this->SetSame();
	typename TestFixture::container v = this->x;
	EXPECT_EQ(this->x.size(), v.size());

	for(unsigned int i = 0; i < v.size(); ++i)
		EXPECT_EQ(this->x[i], v[i]);
}

// --- Copy Assignment ---

TYPED_TEST(DequeTest, CopyAssignment) {
	this->SetDifferent();
	ASSERT_EQ(3, this->y[5]);

	this->y = this->x;

	ASSERT_EQ(this->x.size(), this->y.size());
	for (unsigned int i = 0; i < this->x.size(); ++i)
		EXPECT_EQ(this->x[i], this->y[i]);
}

// --- operator == ---

TYPED_TEST(DequeTest, ContentEqualsOnEmpty) {
	EXPECT_EQ(this->x, this->y);
}

TYPED_TEST(DequeTest, ContentEqualsSmall) {
	this->SetSame();
	EXPECT_EQ(this->x, this->y);
}

TYPED_TEST(DequeTest, ContentEqualsLarge) {
	this->SetLarge();
	EXPECT_EQ(this->x, this->y);
}

TYPED_TEST(DequeTest, ContentNotEqual) {
	this->SetDifferent();
	// EXPECT_NE(this->x, this->y);
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
	this->y.at(this->s - 1) = 0;
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
		typedef typename C::difference_type difference_type;
		C x, y;
		iterator i, j;
		value_type m, n;
		const difference_type s;

		IteratorTest() : s(10000) {}

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

		void Push() {
			for (unsigned int count = 0; count < s; ++count)
				x.push_front(9);
			for (unsigned int count = 0; count < s; ++count)
				x.push_back(9);
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

TYPED_TEST(IteratorTest, IteratorPreDecrement) {
	this->SetUpEnd();
	ASSERT_TRUE((this->x.end() - 1) == (--(this->i)));
}

TYPED_TEST(IteratorTest, IteratorPreDecrementNoTemp) {
	this->SetUpEnd();
	typename TestFixture::iterator ti = --(this->i);
	ASSERT_TRUE(ti == (this->i));
}

// --- iterator --- valid through push_back and push_front ---

TYPED_TEST(IteratorTest, IteratorValidTest) {
	this->SetUpBegin();
	const typename TestFixture::iterator v = this->i;
	this->Push();
	ASSERT_TRUE(v == this->i);
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

// --- const_iterator --- valid through push_back and push_front ---

TYPED_TEST(IteratorTest, ConstIteratorValidTest) {
	this->SetUpBegin();
	const typename TestFixture::const_iterator v = this->i;
	typename TestFixture::const_iterator ci = this->i;
	this->Push();
	ASSERT_TRUE(v == ci);
}


// --- begin ---

TYPED_TEST(IteratorTest, Begin) {
	this->SetUpBegin();
	EXPECT_EQ(0, *(this->x.begin()));
	ASSERT_EQ(&*(this->x.begin()), &(this->x)[0]);
}

TYPED_TEST(IteratorTest, BeginLarge) {
	this->SetUpBegin();
	this->Push();
	EXPECT_EQ(9, *(this->x.begin()));
	ASSERT_EQ(&*(this->x.begin()), &(this->x)[0]);
}

// --- end ---

TYPED_TEST(IteratorTest, End) {
	this->SetUpBegin();
	EXPECT_EQ(2, *(this->x.end() - 1));
	ASSERT_EQ(&*(this->x.end()), ((&(this->x)[(this->x.size() - 1)]) + 1));
}

TYPED_TEST(IteratorTest, EndLarge) {
	this->SetUpBegin();
	this->Push();
	EXPECT_EQ(9, *(this->x.end() - 1));
	ASSERT_EQ(&*((this->x.end()) - 1), (&(this->x)[(this->x.size() - 1)]));
}

// --- erase ---

TYPED_TEST(IteratorTest, Erase) {
	this->SetUpBegin();
	EXPECT_EQ(3, this->x.size());
	this->x.erase(this->i);
	EXPECT_EQ(2, this->x.size());
	EXPECT_EQ(1, *(this->x.begin()));
}

TYPED_TEST(IteratorTest, EraseLarge) {
	this->SetUpBegin();
	this->Push();
	typename TestFixture::difference_type expectedSize = 3 + 2 * this->s;
	EXPECT_EQ(expectedSize, this->x.size());
	EXPECT_EQ(0, *(this->i));
	this->x.erase(this->x.begin() + this->s);
	EXPECT_EQ(expectedSize - 1, this->x.size());
}

// --- insert ---
// TODO
TYPED_TEST(IteratorTest, InsertEmpty) {
	EXPECT_EQ(0, this->x.size());
	this->x.insert(this->x.begin(), 1);
	EXPECT_EQ(1, *(this->x.begin()));
}

TYPED_TEST(IteratorTest, Insert) {
	this->SetUpBegin();
	EXPECT_EQ(3, this->x.size());
	this->x.insert(this->i, 7);
	EXPECT_EQ(4, this->x.size());
	EXPECT_EQ(7, this->x[0]);
}

// --- MyDeque Implementation Tests ---
// These are tests tailored to MyDeque
// Here, I can test implementation-dependent details of MyDeque

class MyDequeTest : public testing::Test {
	protected:
		typedef MyDeque<int> container;
		typedef container::value_type value_type;
		typedef container::iterator iterator;
		typedef container::const_iterator const_iterator;
		typedef container::size_type size_type;
		typedef container::difference_type difference_type;
		typedef container::allocator_type allocator_type;
		typedef value_type* pointer;
		typedef pointer* map_pointer;
		container x;
		iterator i;
		value_type v;
		allocator_type a;
		const size_type small, large;

		MyDequeTest() : v(5), small(10), large(10000) {}

		void CleanX() {
			// They're just ints, so no need to call a destructor
			if (x.myMapSize != 0) {
				map_pointer b = x.myMap;
				if (b != NULL) {
					const map_pointer e = b + x.myMapSize;

					while (b != e) {
						if (*b != NULL)
							x.myAllocator.deallocate(*b, container::ROW_SIZE);
					}
					x.myMapAllocator.deallocate(x.myMap, x.myMapSize);
				}
			}
		}
};

// --- default constructor ---

TEST_F(MyDequeTest, DefaultConstructor) {
	EXPECT_EQ(0, x.size());
	EXPECT_EQ(a, x.myAllocator);
}

// --- single allocator constructor ---

TEST_F(MyDequeTest, SingleAllocatrConstructor) {
	container y (a);
	EXPECT_EQ(0, y.size());
	EXPECT_EQ(a, y.myAllocator);
}

// --- size / value constructor ---

TEST_F(MyDequeTest, SizeValueConstructorEmpty) {
	container y (0, v);
	EXPECT_EQ(0, y.size());
	EXPECT_EQ(a, y.myAllocator);
	// TODO stuff for empty MyDeque
}

TEST_F(MyDequeTest, SizeValueConstructorSmall) {
	container y (small, v);
	EXPECT_EQ(small, y.size());
	EXPECT_EQ(a, y.myAllocator);
	// TODO stuff for empty MyDeque
}

TEST_F(MyDequeTest, SizeValueConstructorSmallWithAllocator) {
	container y (small, v, a);
	EXPECT_EQ(small, y.size());
	EXPECT_EQ(a, y.myAllocator);
	// TODO stuff for empty MyDeque
}

TEST_F(MyDequeTest, SizeValueConstructorLarge) {
	container y (large, v);
	EXPECT_EQ(large, y.size());
	EXPECT_EQ(a, y.myAllocator);
	// TODO stuff for empty MyDeque
}

TEST_F(MyDequeTest, SizeValueConstructorLargeWithAllocator) {
	container y (large, v, a);
	EXPECT_EQ(large, y.size());
	EXPECT_EQ(a, y.myAllocator);
	// TODO stuff for empty MyDeque
}

// --- copy constructor ---

TEST_F(MyDequeTest, CopyConstructorEmpty) {
	container z (x);
	EXPECT_EQ(0, z.size());
	EXPECT_EQ(a, z.myAllocator);

	// TODO stuff for empty MyDeque
}

TEST_F(MyDequeTest, CopyConstructorSmall) {
	container y (small, v);
	EXPECT_EQ(small, y.size());
	EXPECT_EQ(a, y.myAllocator);

	container z (y);
	EXPECT_EQ(small, z.size());
	EXPECT_EQ(a, z.myAllocator);
	// TODO stuff for empty MyDeque
}

TEST_F(MyDequeTest, CopyConstructorLarge) {
	container y (large, v);
	EXPECT_EQ(large, y.size());
	EXPECT_EQ(a, y.myAllocator);

	container z (y);
	EXPECT_EQ(large, z.size());
	EXPECT_EQ(a, z.myAllocator);
	// TODO stuff for empty MyDeque
}

// --- valid ---
// TODO

// --- iterator::valid ---
// TODO

// --- const_iterator::valid ---
// TODO

// --- allocateRow ---

TEST_F(MyDequeTest, AllocateRow) {
	const pointer p = x.allocateRow();
	ASSERT_NE(static_cast<const pointer>(NULL), p);
	x.myAllocator.deallocate(p, container::ROW_SIZE);
}


// --- deallocateRow ---

TEST_F(MyDequeTest, DellocateRow) {
	const pointer p = x.myAllocator.allocate(container::ROW_SIZE);
	ASSERT_NE(static_cast<const pointer>(NULL), p);
	x.deallocateRow(p);
}

// --- allocateMap ---

TEST_F(MyDequeTest, AllocateMapZero) {
	const map_pointer p = x.allocateMap(0);
	ASSERT_NE(static_cast<const map_pointer>(NULL), p);
	x.myMapAllocator.deallocate(p, 0);
}

TEST_F(MyDequeTest, AllocateMapSmall) {
	const map_pointer p = x.allocateMap(small);
	ASSERT_NE(static_cast<const map_pointer>(NULL), p);
	x.myMapAllocator.deallocate(p, small);
}

TEST_F(MyDequeTest, AllocateMapLarge) {
	const map_pointer p = x.allocateMap(large);
	ASSERT_NE(static_cast<const map_pointer>(NULL), p);
	x.myMapAllocator.deallocate(p, large);
}

// --- deallocateMap ---

TEST_F(MyDequeTest, DeallocateMapZero) {
	const map_pointer p = x.myMapAllocator.allocate(0);
	ASSERT_NE(static_cast<const map_pointer>(NULL), p);
	x.deallocateMap(p, 0);
}

TEST_F(MyDequeTest, DeallocateMapSmall) {
	map_pointer p = x.myMapAllocator.allocate(small);
	ASSERT_NE(static_cast<const map_pointer>(NULL), p);
	x.myMapAllocator.deallocate(p, small);
}

TEST_F(MyDequeTest, DeallocateMapLarge) {
	const map_pointer p = x.myMapAllocator.allocate(large);
	ASSERT_NE(static_cast<const map_pointer>(NULL), p);
	x.myMapAllocator.deallocate(p, large);
}

// --- resizeMap ---

TEST_F(MyDequeTest, ResizeMapEmpty) {
	x.resizeMap(0);
	EXPECT_EQ(0, x.myMapSize);
}

TEST_F(MyDequeTest, ResizeMapSmall) {
	x.resizeMap(small);
	EXPECT_EQ(small, x.myMapSize);
}

TEST_F(MyDequeTest, ResizeMapLarge) {
	x.resizeMap(large);

	EXPECT_EQ(large, x.myMapSize);
}

TEST_F(MyDequeTest, ResizeMapSameSize) {
	x.resizeMap(small);
	EXPECT_EQ(small, x.myMapSize);
	const map_pointer oldMyMap = x.myMap;
	x.resizeMap(small);
	EXPECT_EQ(small, x.myMapSize);
	EXPECT_EQ(oldMyMap, x.myMap);
}

TEST_F(MyDequeTest, ResizeMapSmaller) {
	const size_type smaller = small - 3;
	x.resizeMap(small);
	ASSERT_EQ(small, x.myMapSize);
	std::vector<pointer> copy (x.myMap, x.myMap + small);

	x.resizeMap(smaller);
	EXPECT_EQ(smaller, x.myMapSize);
	for (unsigned int k = 0; k < smaller; ++k)
		EXPECT_EQ(copy[k], *(x.myMap + k));
}

// --- growMap ---

TEST_F(MyDequeTest, GrowMap) {
	x.myMap = x.myMapAllocator.allocate(1);
	*(x.myMap) = x.myAllocator.allocate(container::ROW_SIZE);
	const size_type s = 3;

	x.growMap();

	EXPECT_EQ(s, x.myMapSize);
}

TEST_F(MyDequeTest, GrowMapTwice) {
	x.myMap = x.myMapAllocator.allocate(1);
	*(x.myMap) = x.myAllocator.allocate(container::ROW_SIZE);
	const size_type s = 9;

	x.growMap();
	x.growMap();

	EXPECT_EQ(s, x.myMapSize);
}

TEST_F(MyDequeTest, GrowMapRepeated) {
	x.myMap = x.myMapAllocator.allocate(1);
	*(x.myMap) = x.myAllocator.allocate(container::ROW_SIZE);
	size_type baseS = 3;
	size_type numGrows = 10;
	size_type s = std::pow(baseS, numGrows);

	for(size_type k = 0; k < numGrows; ++k) {
		x.growMap();
	}	

	EXPECT_EQ(s, x.myMapSize);
}
