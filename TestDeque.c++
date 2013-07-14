// TestDeque

#include <algorithm> // equal
#include <cstring>   // strcmp
#include <deque>     // deque
#include <sstream>   // ostringstream
#include <stdexcept> // invalid_argument
#include <string>    // ==

// Stuff in deque.h so our defines won't mess with stuff
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
		C x, y;

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

	for (int i = 0; i < 100; ++i) {
		this->x.push_back(i);
	}
	for (int i = 0; i < 100; ++i) {
		this->x.pop_back();
	}

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

// --- operator < ---

TYPED_TEST(DequeTest, LessThanEqualEmpty) {
	EXPECT_LE(this->x, this->y);
}

TYPED_TEST(DequeTest, LessThan) {
	this->SetLessContent();
	EXPECT_LT(this->x, this->y);
}

TYPED_TEST(DequeTest, LessThanEqual) {
	this->SetLessContent();
	EXPECT_LE(this->x, this->y);
}

TYPED_TEST(DequeTest, LessThanEqualSelf) {
	this->SetLessContent();
	EXPECT_LE(this->x, this->x);
}

TYPED_TEST(DequeTest, LessThanSize) {
	this->SetLessSize();
	EXPECT_LE(this->x, this->y);
}

TYPED_TEST(DequeTest, LessThanEqualSize) {
	this->SetLessSize();
	EXPECT_LE(this->x, this->y);
}

TYPED_TEST(DequeTest, GreaterThanEqualEmpty) {
	EXPECT_GE(this->y, this->x);
}

TYPED_TEST(DequeTest, GreaterThan) {
	this->SetLessContent();
	EXPECT_GT(this->y, this->x);
}

TYPED_TEST(DequeTest, GreaterThanEqual) {
	this->SetLessContent();
	EXPECT_GE(this->y, this->x);
}

TYPED_TEST(DequeTest, GreaterThanEqualSelf) {
	this->SetLessContent();
	EXPECT_GE(this->x, this->x);
}

TYPED_TEST(DequeTest, GreaterThanSize) {
	this->SetLessSize();
	EXPECT_GT(this->y, this->x);
}

TYPED_TEST(DequeTest, GreaterThanEqualSize) {
	this->SetLessSize();
	EXPECT_GE(this->y, this->x);
}

// --- Allocator Constructor ---

TYPED_TEST(DequeTest, AllocatorConstructorEmpty) {
	EXPECT_EQ(0, this->x.size());
	EXPECT_EQ(0, this->y.size());
}



// --- Iterator Interface Tests ---
// Test the iterators of both classes

template<typename C>
class IteratorTest : public testing::Test {
	protected:
		typedef typename C::value_type T;
		typedef typename C::iterator I;
		C x, y;
		I i, j;
};

TYPED_TEST_CASE(IteratorTest, MyDeques);


// --- MyDeque Implementation Tests ---
// These are tests tailored to MyDeque

template<typename T>
class MyDequeTest : public testing::Test {
	protected:
		MyDeque<T> x, y;
};

typedef testing::Types<int, char, std::string> MyTypes;
TYPED_TEST_CASE(MyDequeTest, MyTypes);
