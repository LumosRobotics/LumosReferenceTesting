#include <gtest/gtest.h>
#include "reference_testing/reference_testing.h"

using namespace lumos;

class VarianceThresholdTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ref_vec = {1.0, 2.0, 3.0, 4.0, 5.0};
        identical_vec = {1.0, 2.0, 3.0, 4.0, 5.0};
        small_diff_vec = {1.01, 1.99, 3.02, 3.98, 5.01};
        large_diff_vec = {2.0, 4.0, 6.0, 8.0, 10.0};
    }

    std::vector<double> ref_vec, identical_vec, small_diff_vec, large_diff_vec;
};

TEST_F(VarianceThresholdTest, IdenticalVectors)
{
    EXPECT_TRUE(isVarianceWithinThreshold(identical_vec, ref_vec, 0.0));
    EXPECT_TRUE(isVarianceWithinThreshold(ref_vec, ref_vec, 0.0));
}

TEST_F(VarianceThresholdTest, SmallDifferences)
{
    EXPECT_TRUE(isVarianceWithinThreshold(small_diff_vec, ref_vec, 0.01));
    EXPECT_FALSE(isVarianceWithinThreshold(small_diff_vec, ref_vec, 0.0001));
}

TEST_F(VarianceThresholdTest, LargeDifferences)
{
    EXPECT_FALSE(isVarianceWithinThreshold(large_diff_vec, ref_vec, 0.1));
    EXPECT_TRUE(isVarianceWithinThreshold(large_diff_vec, ref_vec, 50.0));
}

TEST_F(VarianceThresholdTest, EmptyVectors)
{
    std::vector<double> empty_vec;
    EXPECT_TRUE(isVarianceWithinThreshold(empty_vec, empty_vec, 0.0));
}

TEST_F(VarianceThresholdTest, SizeMismatch)
{
    std::vector<double> short_vec = {1.0, 2.0};
    EXPECT_FALSE(isVarianceWithinThreshold(short_vec, ref_vec, 1.0));
}

TEST_F(VarianceThresholdTest, FloatType)
{
    std::vector<float> ref_float = {1.0f, 2.0f, 3.0f};
    std::vector<float> test_float = {1.1f, 2.1f, 3.1f};

    EXPECT_TRUE(isVarianceWithinThreshold(test_float, ref_float, 0.1f));
}