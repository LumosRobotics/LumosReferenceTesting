#include <gtest/gtest.h>
#include "reference_testing/reference_testing.h"

using namespace lumos;

class IsWithinBoundsTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        test_vec = {1.5, 2.5, 3.5, 4.5};
        min_bounds = {1.0, 2.0, 3.0, 4.0};
        max_bounds = {2.0, 3.0, 4.0, 5.0};

        time_vec = {0.0, 1.0, 2.0, 3.0};
        min_time = {0.0, 1.0, 2.0};
        max_time = {0.0, 1.0, 2.0};
        min_values = {0.5, 2.0, 3.0};
        max_values = {2.0, 3.0, 4.5};
    }

    std::vector<double> test_vec, min_bounds, max_bounds;
    std::vector<double> time_vec, min_time, max_time, min_values, max_values;
};

TEST_F(IsWithinBoundsTest, SimpleWithinBounds)
{
    EXPECT_TRUE(isWithinBounds(test_vec, min_bounds, max_bounds));

    std::vector<double> out_of_bounds = {0.5, 2.5, 3.5, 4.5};
    EXPECT_FALSE(isWithinBounds(out_of_bounds, min_bounds, max_bounds));
}

TEST_F(IsWithinBoundsTest, ExactBoundaryValues)
{
    std::vector<double> on_min_bound = {1.0, 2.0, 3.0, 4.0};
    std::vector<double> on_max_bound = {2.0, 3.0, 4.0, 5.0};

    EXPECT_TRUE(isWithinBounds(on_min_bound, min_bounds, max_bounds));
    EXPECT_TRUE(isWithinBounds(on_max_bound, min_bounds, max_bounds));
}

TEST_F(IsWithinBoundsTest, SizeMismatch)
{
    std::vector<double> short_vec = {1.5, 2.5};
    EXPECT_FALSE(isWithinBounds(short_vec, min_bounds, max_bounds));

    std::vector<double> short_bounds = {1.0, 2.0};
    EXPECT_FALSE(isWithinBounds(test_vec, short_bounds, max_bounds));
}

TEST_F(IsWithinBoundsTest, EmptyVectors)
{
    std::vector<double> empty_vec;
    std::vector<double> empty_bounds;

    EXPECT_TRUE(isWithinBounds(empty_vec, empty_bounds, empty_bounds));
}

TEST_F(IsWithinBoundsTest, TimeBasedWithinBounds)
{
    std::vector<double> test_values = {1.0, 2.5, 3.2, 4.0};

    EXPECT_TRUE(isWithinBounds(time_vec, test_values, min_time, min_values, max_time, max_values));
}

TEST_F(IsWithinBoundsTest, TimeBasedSizeMismatch)
{
    std::vector<double> test_values = {1.0, 2.5};

    EXPECT_FALSE(isWithinBounds(time_vec, test_values, min_time, min_values, max_time, max_values));
}