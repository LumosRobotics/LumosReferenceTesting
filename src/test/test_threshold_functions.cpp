#include <gtest/gtest.h>
#include "../applications/simple/bounds_checker.hpp"

class ThresholdFunctionsTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        mixed_vec = {0.5, 1.5, 0.3, 2.1, 2.8, 0.7, 3.2, 3.5, 0.9, 2.9};
        consecutive_above = {0.5, 0.3, 2.1, 2.8, 2.7, 0.7, 3.2, 3.5, 3.1, 0.9};
        consecutive_below = {3.5, 0.2, 0.1, 0.3, 2.8, 0.4, 0.1, 0.2, 3.1, 0.9};
    }

    std::vector<double> mixed_vec, consecutive_above, consecutive_below;
};

TEST_F(ThresholdFunctionsTest, AtLeastNSamplesAboveThreshold) 
{
    EXPECT_TRUE(hasAtLeastNSamplesAboveThreshold(mixed_vec, 1.0, 5));
    EXPECT_FALSE(hasAtLeastNSamplesAboveThreshold(mixed_vec, 3.0, 3));
    EXPECT_TRUE(hasAtLeastNSamplesAboveThreshold(mixed_vec, 2.0, 4));
    
    // Edge cases
    std::vector<double> empty_vec;
    EXPECT_FALSE(hasAtLeastNSamplesAboveThreshold(empty_vec, 1.0, 1));
    EXPECT_TRUE(hasAtLeastNSamplesAboveThreshold(mixed_vec, 1.0, 0));
}

TEST_F(ThresholdFunctionsTest, AtLeastNConsecutiveSamplesAboveThreshold) 
{
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesAboveThreshold(consecutive_above, 2.0, 3));
    EXPECT_FALSE(hasAtLeastNConsecutiveSamplesAboveThreshold(consecutive_above, 2.0, 4));
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesAboveThreshold(consecutive_above, 3.0, 3));
    
    // Edge cases
    std::vector<double> empty_vec;
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesAboveThreshold(empty_vec, 1.0, 0));
    EXPECT_FALSE(hasAtLeastNConsecutiveSamplesAboveThreshold(empty_vec, 1.0, 1));
}

TEST_F(ThresholdFunctionsTest, AtLeastNSamplesBelowThreshold) 
{
    EXPECT_TRUE(hasAtLeastNSamplesBelowThreshold(mixed_vec, 1.0, 3));
    EXPECT_FALSE(hasAtLeastNSamplesBelowThreshold(mixed_vec, 0.4, 2));
    EXPECT_TRUE(hasAtLeastNSamplesBelowThreshold(mixed_vec, 2.0, 4));
    
    // Edge cases
    std::vector<double> empty_vec;
    EXPECT_FALSE(hasAtLeastNSamplesBelowThreshold(empty_vec, 1.0, 1));
    EXPECT_TRUE(hasAtLeastNSamplesBelowThreshold(mixed_vec, 1.0, 0));
}

TEST_F(ThresholdFunctionsTest, AtLeastNConsecutiveSamplesBelowThreshold) 
{
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesBelowThreshold(consecutive_below, 1.0, 3));
    EXPECT_FALSE(hasAtLeastNConsecutiveSamplesBelowThreshold(consecutive_below, 0.5, 4));
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesBelowThreshold(consecutive_below, 0.5, 3));
    
    // Edge cases
    std::vector<double> empty_vec;
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesBelowThreshold(empty_vec, 1.0, 0));
    EXPECT_FALSE(hasAtLeastNConsecutiveSamplesBelowThreshold(empty_vec, 1.0, 1));
}

TEST_F(ThresholdFunctionsTest, FloatTypes) 
{
    std::vector<float> float_vec = {0.5f, 1.5f, 2.5f, 3.5f};
    
    EXPECT_TRUE(hasAtLeastNSamplesAboveThreshold(float_vec, 1.0f, 2));
    EXPECT_TRUE(hasAtLeastNSamplesBelowThreshold(float_vec, 2.0f, 2));
}