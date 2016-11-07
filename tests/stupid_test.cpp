#include <iostream>
#include "../lib/googletest/googletest/include/gtest/gtest.h"

TEST(IndependentMethod, DoesNothing){
  EXPECT_EQ(0, 0);
}
