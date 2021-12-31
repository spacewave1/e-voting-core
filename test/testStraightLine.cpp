//
// Created by wnabo on 30.12.2021.
//

#include "../src/straightLineDistributeThread.h"
#include <gtest/gtest.h>

TEST(distribution_invert_direction_up, DemonstrateGTestMacros) {
    straightLineDistributeThread testee;
    EXPECT_EQ(testee.invertDirection("up"),"down");
}

TEST(distribution_invert_direction_down, DemonstrateGTestMacros) {
    straightLineDistributeThread testee;
    EXPECT_EQ(testee.invertDirection("down"),"up");
}