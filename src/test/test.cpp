#include <gtest/gtest.h>
#include <vector>
#include <cmath>
#include "../applications/simple/bounds_checker.hpp"
#include "../applications/simple/binary_serializer.hpp"

class BoundsCheckerTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        // Set up test data
        test_vec = {1.01, 1.99, 3.02, 3.98, 5.01};
        ref_vec = {1.0, 2.0, 3.0, 4.0, 5.0};
        min_bounds = {0.5, 1.5, 2.5, 3.5, 4.5};
        max_bounds = {1.5, 2.5, 3.5, 4.5, 5.5};
    }

    std::vector<double> test_vec;
    std::vector<double> ref_vec;
    std::vector<double> min_bounds;
    std::vector<double> max_bounds;
};

TEST_F(BoundsCheckerTest, IsWithinBounds) 
{
    EXPECT_TRUE(isWithinBounds(test_vec, min_bounds, max_bounds));
    
    std::vector<double> out_of_bounds = {0.1, 2.0, 3.0, 4.0, 5.0};
    EXPECT_FALSE(isWithinBounds(out_of_bounds, min_bounds, max_bounds));
}

TEST_F(BoundsCheckerTest, VarianceWithinThreshold) 
{
    EXPECT_TRUE(isVarianceWithinThreshold(test_vec, ref_vec, 0.01));
    EXPECT_FALSE(isVarianceWithinThreshold(test_vec, ref_vec, 0.0001));
    
    // Identical vectors should have zero variance
    EXPECT_TRUE(isVarianceWithinThreshold(ref_vec, ref_vec, 0.0));
}

TEST_F(BoundsCheckerTest, MeanDifferenceWithinThreshold) 
{
    EXPECT_TRUE(isMeanDifferenceWithinThreshold(test_vec, ref_vec, 0.1));
    EXPECT_FALSE(isMeanDifferenceWithinThreshold(test_vec, ref_vec, 0.001));
    
    // Identical vectors should have zero mean difference
    EXPECT_TRUE(isMeanDifferenceWithinThreshold(ref_vec, ref_vec, 0.0));
}

TEST_F(BoundsCheckerTest, AtLeastNSamplesAboveThreshold) 
{
    std::vector<double> mixed_vec = {0.5, 1.5, 0.3, 2.1, 2.8, 0.7, 3.2, 3.5, 0.9, 2.9};
    
    EXPECT_TRUE(hasAtLeastNSamplesAboveThreshold(mixed_vec, 1.0, 5));
    EXPECT_FALSE(hasAtLeastNSamplesAboveThreshold(mixed_vec, 3.0, 3));
    EXPECT_TRUE(hasAtLeastNSamplesAboveThreshold(mixed_vec, 2.0, 4));
}

TEST_F(BoundsCheckerTest, AtLeastNConsecutiveSamplesAboveThreshold) 
{
    std::vector<double> consecutive_vec = {0.5, 0.3, 2.1, 2.8, 2.7, 0.7, 3.2, 3.5, 3.1, 0.9};
    
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesAboveThreshold(consecutive_vec, 2.0, 3));
    EXPECT_FALSE(hasAtLeastNConsecutiveSamplesAboveThreshold(consecutive_vec, 2.0, 4));
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesAboveThreshold(consecutive_vec, 3.0, 3));
}

TEST_F(BoundsCheckerTest, AtLeastNSamplesBelowThreshold) 
{
    std::vector<double> mixed_vec = {0.5, 1.5, 0.3, 2.1, 2.8, 0.7, 3.2, 3.5, 0.9, 2.9};
    
    EXPECT_TRUE(hasAtLeastNSamplesBelowThreshold(mixed_vec, 1.0, 3));
    EXPECT_FALSE(hasAtLeastNSamplesBelowThreshold(mixed_vec, 0.4, 2));
    EXPECT_TRUE(hasAtLeastNSamplesBelowThreshold(mixed_vec, 2.0, 4));
}

TEST_F(BoundsCheckerTest, AtLeastNConsecutiveSamplesBelowThreshold) 
{
    std::vector<double> consecutive_vec = {3.5, 0.2, 0.1, 0.3, 2.8, 0.4, 0.1, 0.2, 3.1, 0.9};
    
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesBelowThreshold(consecutive_vec, 1.0, 3));
    EXPECT_FALSE(hasAtLeastNConsecutiveSamplesBelowThreshold(consecutive_vec, 0.5, 4));
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesBelowThreshold(consecutive_vec, 0.5, 3));
}

TEST_F(BoundsCheckerTest, CustomConditionFunctions) 
{
    std::vector<double> condition_vec = {-1.5, 2.5, -0.5, 3.5, 1.5, -2.5, 4.5};
    
    auto is_positive = [](const double& x) { return x > 0; };
    EXPECT_TRUE(hasAtLeastNSamplesWithConditionTrue(condition_vec, is_positive, 4));
    EXPECT_FALSE(hasAtLeastNSamplesWithConditionTrue(condition_vec, is_positive, 5));
    
    auto above_two = [](const double& x) { return x > 2.0; };
    std::vector<double> consecutive_condition_vec = {0.5, 2.5, 2.8, 2.1, 0.3, 4.1, 4.5, 4.8, 1.2};
    EXPECT_TRUE(hasAtLeastNConsecutiveSamplesWithConditionTrue(consecutive_condition_vec, above_two, 3));
    EXPECT_FALSE(hasAtLeastNConsecutiveSamplesWithConditionTrue(consecutive_condition_vec, above_two, 4));
}

TEST_F(BoundsCheckerTest, LinearInterpolation) 
{
    EXPECT_DOUBLE_EQ(linearInterpolate(1.5, 1.0, 2.0, 2.0, 4.0), 3.0);
    EXPECT_DOUBLE_EQ(linearInterpolate(0.0, 0.0, 5.0, 1.0, 10.0), 5.0);
    EXPECT_DOUBLE_EQ(linearInterpolate(0.5, 0.0, 0.0, 1.0, 10.0), 5.0);
}

class BinarySerializerTest : public ::testing::Test 
{
protected:
    void TearDown() override 
    {
        // Clean up test files
        std::remove("test_vector.bin");
    }
};

TEST_F(BinarySerializerTest, SaveAndLoadVector) 
{
    std::vector<double> original = {1.1, 2.2, 3.3, 4.4, 5.5};
    
    // Save vector
    EXPECT_NO_THROW(saveBinaryVector(original, "test_vector.bin"));
    
    // Load vector
    std::vector<double> loaded;
    EXPECT_NO_THROW(loaded = loadBinaryVector<double>("test_vector.bin"));
    
    // Verify data
    ASSERT_EQ(original.size(), loaded.size());
    for (size_t i = 0; i < original.size(); ++i) 
    {
        EXPECT_DOUBLE_EQ(original[i], loaded[i]);
    }
}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}