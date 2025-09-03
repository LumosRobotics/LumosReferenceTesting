#include <gtest/gtest.h>
#include "reference_testing/reference_testing.h"

using namespace lumos;

class Corridor2DTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create a simple rectangular corridor
        x_left = {0.0, 0.0, 0.0, 0.0};
        y_left = {0.0, 1.0, 2.0, 3.0};
        x_right = {2.0, 2.0, 2.0, 2.0};
        y_right = {0.0, 1.0, 2.0, 3.0};

        // Test points inside corridor
        x_inside = {1.0, 1.0, 1.0};
        y_inside = {0.5, 1.5, 2.5};

        // Test points outside corridor
        x_outside = {-1.0, 3.0, 1.0};
        y_outside = {1.0, 1.0, -1.0};
    }

    std::vector<double> x_left, y_left, x_right, y_right;
    std::vector<double> x_inside, y_inside, x_outside, y_outside;
};

TEST_F(Corridor2DTest, PointsInsideCorridor)
{
    EXPECT_TRUE(isWithin2DCorridor(x_inside, y_inside, x_left, y_left, x_right, y_right));
}

TEST_F(Corridor2DTest, PointsOutsideCorridor)
{
    EXPECT_FALSE(isWithin2DCorridor(x_outside, y_outside, x_left, y_left, x_right, y_right));
}

TEST_F(Corridor2DTest, BoundaryPoints)
{
    std::vector<double> x_boundary = {0.0, 2.0, 1.0};
    std::vector<double> y_boundary = {1.0, 1.0, 0.0};

    EXPECT_TRUE(isWithin2DCorridor(x_boundary, y_boundary, x_left, y_left, x_right, y_right));
}

TEST_F(Corridor2DTest, ErrorCases)
{
    std::vector<double> mismatched_x = {1.0, 2.0};
    std::vector<double> mismatched_y = {1.0, 2.0, 3.0};

    EXPECT_THROW(isWithin2DCorridor(mismatched_x, mismatched_y, x_left, y_left, x_right, y_right),
                 std::invalid_argument);

    std::vector<double> single_point = {1.0};
    EXPECT_THROW(isWithin2DCorridor(x_inside, y_inside, single_point, single_point, x_right, y_right),
                 std::invalid_argument);
}

TEST_F(Corridor2DTest, EmptyTrajectory)
{
    std::vector<double> empty_x, empty_y;
    EXPECT_TRUE(isWithin2DCorridor(empty_x, empty_y, x_left, y_left, x_right, y_right));
}

TEST_F(Corridor2DTest, SimpleRectangularCorridor)
{
    // Additional test with clearly defined inside/outside points
    std::vector<double> clearly_inside_x = {1.0, 1.0};
    std::vector<double> clearly_inside_y = {1.0, 2.0};

    EXPECT_TRUE(isWithin2DCorridor(clearly_inside_x, clearly_inside_y, x_left, y_left, x_right, y_right));

    std::vector<double> clearly_outside_x = {-0.5, 3.0};
    std::vector<double> clearly_outside_y = {1.0, 1.0};

    EXPECT_FALSE(isWithin2DCorridor(clearly_outside_x, clearly_outside_y, x_left, y_left, x_right, y_right));
}