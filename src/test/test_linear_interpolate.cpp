#include <gtest/gtest.h>
#include "../applications/simple/bounds_checker.hpp"

class LinearInterpolateTest : public ::testing::Test {};

TEST_F(LinearInterpolateTest, BasicInterpolation) 
{
    EXPECT_DOUBLE_EQ(linearInterpolate(1.5, 1.0, 2.0, 2.0, 4.0), 3.0);
    EXPECT_DOUBLE_EQ(linearInterpolate(0.5, 0.0, 0.0, 1.0, 10.0), 5.0);
    EXPECT_DOUBLE_EQ(linearInterpolate(0.25, 0.0, 0.0, 1.0, 8.0), 2.0);
}

TEST_F(LinearInterpolateTest, EdgeCases) 
{
    // Same x values should return y0
    EXPECT_DOUBLE_EQ(linearInterpolate(1.0, 1.0, 5.0, 1.0, 10.0), 5.0);
    EXPECT_DOUBLE_EQ(linearInterpolate(2.0, 1.0, 5.0, 1.0, 10.0), 5.0);
    
    // Extrapolation beyond range
    EXPECT_DOUBLE_EQ(linearInterpolate(-1.0, 0.0, 0.0, 1.0, 10.0), -10.0);
    EXPECT_DOUBLE_EQ(linearInterpolate(2.0, 0.0, 0.0, 1.0, 10.0), 20.0);
}

TEST_F(LinearInterpolateTest, NegativeValues) 
{
    EXPECT_DOUBLE_EQ(linearInterpolate(0.5, 0.0, -5.0, 1.0, 5.0), 0.0);
    EXPECT_DOUBLE_EQ(linearInterpolate(0.25, 0.0, -10.0, 1.0, 10.0), -5.0);
}

TEST_F(LinearInterpolateTest, FloatType) 
{
    EXPECT_FLOAT_EQ(linearInterpolate(1.5f, 1.0f, 2.0f, 2.0f, 4.0f), 3.0f);
    EXPECT_FLOAT_EQ(linearInterpolate(0.5f, 0.0f, 0.0f, 1.0f, 10.0f), 5.0f);
}