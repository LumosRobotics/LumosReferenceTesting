#include <gtest/gtest.h>
#include "../applications/simple/bounds_checker.hpp"

class MeanDifferenceTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        ref_vec = {1.0, 2.0, 3.0, 4.0, 5.0}; // mean = 3.0
        identical_vec = {1.0, 2.0, 3.0, 4.0, 5.0}; // mean = 3.0
        shifted_vec = {2.0, 3.0, 4.0, 5.0, 6.0}; // mean = 4.0, diff = 1.0
        small_diff_vec = {1.1, 2.1, 3.1, 4.1, 5.1}; // mean = 3.1, diff = 0.1
    }

    std::vector<double> ref_vec, identical_vec, shifted_vec, small_diff_vec;
};

TEST_F(MeanDifferenceTest, IdenticalVectors) 
{
    EXPECT_TRUE(isMeanDifferenceWithinThreshold(identical_vec, ref_vec, 0.0));
    EXPECT_TRUE(isMeanDifferenceWithinThreshold(ref_vec, ref_vec, 0.0));
}

TEST_F(MeanDifferenceTest, SmallMeanDifference) 
{
    EXPECT_TRUE(isMeanDifferenceWithinThreshold(small_diff_vec, ref_vec, 0.2));
    EXPECT_FALSE(isMeanDifferenceWithinThreshold(small_diff_vec, ref_vec, 0.05));
}

TEST_F(MeanDifferenceTest, LargeMeanDifference) 
{
    EXPECT_FALSE(isMeanDifferenceWithinThreshold(shifted_vec, ref_vec, 0.5));
    EXPECT_TRUE(isMeanDifferenceWithinThreshold(shifted_vec, ref_vec, 1.5));
}

TEST_F(MeanDifferenceTest, EmptyVectors) 
{
    std::vector<double> empty_vec;
    EXPECT_TRUE(isMeanDifferenceWithinThreshold(empty_vec, empty_vec, 0.0));
}

TEST_F(MeanDifferenceTest, SizeMismatch) 
{
    std::vector<double> short_vec = {1.0, 2.0};
    EXPECT_FALSE(isMeanDifferenceWithinThreshold(short_vec, ref_vec, 1.0));
}

TEST_F(MeanDifferenceTest, NegativeValues) 
{
    std::vector<double> negative_vec = {-1.0, -2.0, -3.0, -4.0, -5.0}; // mean = -3.0
    EXPECT_TRUE(isMeanDifferenceWithinThreshold(negative_vec, ref_vec, 6.1)); // |(-3.0) - 3.0| = 6.0
    EXPECT_FALSE(isMeanDifferenceWithinThreshold(negative_vec, ref_vec, 5.9));
}