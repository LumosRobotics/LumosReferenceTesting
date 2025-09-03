#include <gtest/gtest.h>
#include "reference_testing/reference_testing.h"

using namespace lumos;

class InterpolateAtTimeTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        time_vec = {0.0, 1.0, 2.0, 3.0, 4.0};
        value_vec = {0.0, 10.0, 5.0, 15.0, 20.0};
    }

    std::vector<double> time_vec;
    std::vector<double> value_vec;
};

TEST_F(InterpolateAtTimeTest, ExactTimePoints)
{
    EXPECT_DOUBLE_EQ(interpolateAtTime(0.0, time_vec, value_vec), 0.0);
    EXPECT_DOUBLE_EQ(interpolateAtTime(1.0, time_vec, value_vec), 10.0);
    EXPECT_DOUBLE_EQ(interpolateAtTime(2.0, time_vec, value_vec), 5.0);
    EXPECT_DOUBLE_EQ(interpolateAtTime(4.0, time_vec, value_vec), 20.0);
}

TEST_F(InterpolateAtTimeTest, InterpolatedTimePoints)
{
    EXPECT_DOUBLE_EQ(interpolateAtTime(0.5, time_vec, value_vec), 5.0);
    EXPECT_DOUBLE_EQ(interpolateAtTime(1.5, time_vec, value_vec), 7.5);
    EXPECT_DOUBLE_EQ(interpolateAtTime(2.5, time_vec, value_vec), 10.0);
}

TEST_F(InterpolateAtTimeTest, ExtrapolationBounds)
{
    // Before first time point
    EXPECT_DOUBLE_EQ(interpolateAtTime(-1.0, time_vec, value_vec), 0.0);

    // After last time point
    EXPECT_DOUBLE_EQ(interpolateAtTime(5.0, time_vec, value_vec), 20.0);
}

TEST_F(InterpolateAtTimeTest, ErrorCases)
{
    std::vector<double> empty_time, empty_value;
    std::vector<double> mismatched_time = {1.0, 2.0};
    std::vector<double> mismatched_value = {1.0, 2.0, 3.0};

    EXPECT_THROW(interpolateAtTime(1.0, empty_time, empty_value), std::invalid_argument);
    EXPECT_THROW(interpolateAtTime(1.0, mismatched_time, mismatched_value), std::invalid_argument);
}

TEST_F(InterpolateAtTimeTest, SinglePoint)
{
    std::vector<double> single_time = {1.0};
    std::vector<double> single_value = {5.0};

    EXPECT_DOUBLE_EQ(interpolateAtTime(0.5, single_time, single_value), 5.0);
    EXPECT_DOUBLE_EQ(interpolateAtTime(1.0, single_time, single_value), 5.0);
    EXPECT_DOUBLE_EQ(interpolateAtTime(2.0, single_time, single_value), 5.0);
}