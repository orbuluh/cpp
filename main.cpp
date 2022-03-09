#include "designPattern/20.observer/observerUseCase.h"
#include "designPattern/25.maybeMonad/maybeMonadUseCase.h"
#include <gtest/gtest.h>
#include <iostream>
#include <ranges>

// Demonstrate some basic assertions.
/*TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}*/

int main() {
    //maybemonad::demo();
    observer::demo();
}
