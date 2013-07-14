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

// These are tests that both deques should pass
template <typename C>
class DequeTest : public testing::Test {
	protected:
		C x;
		C y;

		void SetSame() {
			x = C (10, 5);
			y = C (10, 5);
		}

		void SetDifferent() {
			SetSame();
			y[5] = 3;
		}
};

typedef testing::Types<std::deque<int>, MyDeque<int> > MyTypes;
TYPED_TEST_CASE(DequeTest, MyTypes);

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

TYPED_TEST(DequeTest, ContentNotEqualSize) {
	this->SetSame();
	this->x.push_back(0);
	EXPECT_NE(this->x, this->y);
}

template<typename I>
class DequeIteratorTest : public testing::Test {
	protected:
		// Stuff
};
