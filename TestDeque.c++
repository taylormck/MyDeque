// TestDeque

#include <algorithm> // equal
#include <cstring>   // strcmp
#include <deque>     // deque
#include <sstream>   // ostringstream
#include <stdexcept> // invalid_argument
#include <string>    // ==

#include "gtest/gtest.h" // Google Test framework

#define private public
#define protected public
#define class struct

#include "Deque.h"

#define private public
#define protected public
#define class struct

// Not testing the code we didn't write
// destroy, unitialized_copy, unitialized_fill

// These are tests that both deques should pass
template <typename C>
class DequeTest : public testing::Test {
	protected:
		C x;
		C y;
};

typedef ::testing::Types<MyDeque<int>, std::deque<int> > MyTypes;
TYPED_TEST_CASE(DequeTest, MyTypes);

TYPED_TEST(DequeTest, ContentEqualsOnEmpty) {
	ASSERT_EQ(this->x, this->y);
}
