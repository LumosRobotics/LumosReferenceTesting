#include <gtest/gtest.h>
#include "reference_testing/reference_testing.h"

using namespace lumos;

class ConditionFunctionsTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        condition_vec = {-1.5, 2.5, -0.5, 3.5, 1.5, -2.5, 4.5};
        consecutive_condition_vec = {0.5, 2.5, 2.8, 2.1, 0.3, 4.1, 4.5, 4.8, 1.2};
    }

    std::vector<double> condition_vec, consecutive_condition_vec;
};

TEST_F(ConditionFunctionsTest, AtLeastNSamplesWithCondition)
{
    auto is_positive = [](const double &x)
    { return x > 0; };
    EXPECT_TRUE(hasAtLeastNSamplesWithConditionTrue(condition_vec, is_positive, 4));
    EXPECT_FALSE(hasAtLeastNSamplesWithConditionTrue(condition_vec, is_positive, 5));

    auto is_negative = [](const double &x)
    { return x < 0; };
    EXPECT_TRUE(hasAtLeastNSamplesWithConditionTrue(condition_vec, is_negative, 3));
    EXPECT_FALSE(hasAtLeastNSamplesWithConditionTrue(condition_vec, is_negative, 4));
}

TEST_F(ConditionFunctionsTest, AtLeastNConsecutiveSamplesWithCondition)
{
    auto above_two = [](const double &x)
    { return x > 2.0; };
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesWithConditionTrue(consecutive_condition_vec, above_two, 3));
    EXPECT_FALSE(hasAtLeastNConsecutiveSamplesWithConditionTrue(consecutive_condition_vec, above_two, 4));

    auto above_four = [](const double &x)
    { return x > 4.0; };
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesWithConditionTrue(consecutive_condition_vec, above_four, 3));
}

TEST_F(ConditionFunctionsTest, ComplexConditions)
{
    auto is_even_when_cast = [](const double &x)
    { return static_cast<int>(x) % 2 == 0; };
    EXPECT_TRUE(hasAtLeastNSamplesWithConditionTrue(condition_vec, is_even_when_cast, 2));

    auto is_in_range = [](const double &x)
    { return x >= 1.0 && x <= 3.0; };
    EXPECT_TRUE(hasAtLeastNSamplesWithConditionTrue(condition_vec, is_in_range, 2));
}

TEST_F(ConditionFunctionsTest, EmptyVectors)
{
    std::vector<double> empty_vec;
    auto always_true = [](const double &)
    { return true; };

    EXPECT_FALSE(hasAtLeastNSamplesWithConditionTrue(empty_vec, always_true, 1));
    EXPECT_TRUE(hasAtLeastNSamplesWithConditionTrue(empty_vec, always_true, 0));

    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesWithConditionTrue(empty_vec, always_true, 0));
    EXPECT_FALSE(hasAtLeastNConsecutiveSamplesWithConditionTrue(empty_vec, always_true, 1));
}

TEST_F(ConditionFunctionsTest, ZeroConsecutiveRequirement)
{
    auto is_positive = [](const double &x)
    { return x > 0; };
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesWithConditionTrue(condition_vec, is_positive, 0));

    std::vector<double> all_negative = {-1.0, -2.0, -3.0};
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesWithConditionTrue(all_negative, is_positive, 0));
}