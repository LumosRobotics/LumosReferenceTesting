#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <utility>
// #include <nlohmann/json.hpp>
#include "bounds_checker.hpp"
#include "binary_serializer.hpp"

bool ReferenceDataShouldGenerate()
{
  return true;
}

template <typename T>
std::vector<T> GetReferenceData(const std::string &name, const std::vector<T> &value_vec, const T offset_value)
{
  if (!ReferenceDataShouldGenerate())
  {
    return loadBinaryVector<T>(name + ".bin");
  }
  else
  {
    std::vector<T> adjusted_values(value_vec.size());

    for (size_t k = 0; k < value_vec.size(); ++k)
    {
      adjusted_values[k] = value_vec[k] + offset_value;
    }

    saveBinaryVector<T>(adjusted_values, name + ".bin");
    return adjusted_values;
  }
}

#define TEST_METHOD(name) \
  if (!ReferenceDataShouldGenerate())

void EXPECT_TRUE(const bool value, const std::string &message)
{
  if (value)
  {
    std::cout << "\033[32m[PASS]\033[0m " << message << std::endl;
  }
  else
  {
    std::cout << "\033[31m[FAIL]\033[0m " << message << std::endl;
  }
}

constexpr int N = 100;

std::pair<std::vector<double>, std::vector<double>> MethodUnderTest()
{
  std::vector<double> t(N), x(N);
  for (int i = 0; i < N; ++i)
  {
    t[i] = static_cast<double>(i) * 0.01;
    x[i] = std::sin(t[i]);
  }
  return {t, x};
}

void MyTestMethod()
{
  auto [t, x] = MethodUnderTest();

  std::vector<double> x_min = GetReferenceData("x_min", x, -0.1);
  std::vector<double> x_max = GetReferenceData("x_max", x, 0.1);
  std::vector<double> x_ref = GetReferenceData("x_ref", x, 0.0);

  TEST_METHOD("Some name for dvs window")
  {
    EXPECT_TRUE(isWithinBounds(x, x_min, x_max), "x is within bounds");
    EXPECT_TRUE(isVarianceWithinThreshold(x, x_ref, 0.01), "x variance within threshold");
    EXPECT_TRUE(isMeanDifferenceWithinThreshold(x, x_ref, 0.05), "x mean difference within threshold");
  }
}

void TestVarianceAndMeanFunctions()
{
  // Test case 1: Identical vectors should pass all tests
  std::vector<double> identical1 = {1.0, 2.0, 3.0, 4.0, 5.0};
  std::vector<double> identical2 = {1.0, 2.0, 3.0, 4.0, 5.0};

  EXPECT_TRUE(isVarianceWithinThreshold(identical1, identical2, 0.0), "Identical vectors have zero variance");
  EXPECT_TRUE(isMeanDifferenceWithinThreshold(identical1, identical2, 0.0), "Identical vectors have zero mean difference");

  // Test case 2: Small differences should pass with reasonable thresholds
  std::vector<double> test_vec = {1.01, 1.99, 3.02, 3.98, 5.01};
  std::vector<double> ref_vec = {1.0, 2.0, 3.0, 4.0, 5.0};

  EXPECT_TRUE(isVarianceWithinThreshold(test_vec, ref_vec, 0.01), "Small differences pass variance test");
  EXPECT_TRUE(isMeanDifferenceWithinThreshold(test_vec, ref_vec, 0.1), "Small differences pass mean test");

  // Test case 3: Large differences should fail with strict thresholds
  std::vector<double> large_diff = {2.0, 4.0, 6.0, 8.0, 10.0};

  EXPECT_TRUE(!isVarianceWithinThreshold(large_diff, ref_vec, 0.1), "Large differences fail variance test");
  EXPECT_TRUE(!isMeanDifferenceWithinThreshold(large_diff, ref_vec, 0.1), "Large differences fail mean test");

  // Test case 4: Different means but similar variance
  std::vector<double> shifted = {2.0, 3.0, 4.0, 5.0, 6.0}; // All values +1

  EXPECT_TRUE(isVarianceWithinThreshold(shifted, ref_vec, 2.0), "Shifted vector passes loose variance test");
  EXPECT_TRUE(!isMeanDifferenceWithinThreshold(shifted, ref_vec, 0.5), "Shifted vector fails strict mean test");
}

void TestThresholdFunctions()
{
  // Test vector with mixed values
  std::vector<double> mixed_vec = {0.5, 1.5, 0.3, 2.1, 2.8, 0.7, 3.2, 3.5, 0.9, 2.9};

  // Test case 1: At least N samples above threshold
  EXPECT_TRUE(hasAtLeastNSamplesAboveThreshold(mixed_vec, 1.0, 5), "At least 5 samples above 1.0");
  EXPECT_TRUE(!hasAtLeastNSamplesAboveThreshold(mixed_vec, 3.0, 3), "Not 3 samples above 3.0");
  EXPECT_TRUE(hasAtLeastNSamplesAboveThreshold(mixed_vec, 2.0, 4), "At least 4 samples above 2.0");

  // Test case 2: Consecutive samples above threshold
  std::vector<double> consecutive_vec = {0.5, 0.3, 2.1, 2.8, 2.7, 0.7, 3.2, 3.5, 3.1, 0.9};

  EXPECT_TRUE(hasAtLeastNConsecutiveSamplesAboveThreshold(consecutive_vec, 2.0, 3), "3 consecutive above 2.0");
  EXPECT_TRUE(!hasAtLeastNConsecutiveSamplesAboveThreshold(consecutive_vec, 2.0, 4), "Not 4 consecutive above 2.0");
  EXPECT_TRUE(hasAtLeastNConsecutiveSamplesAboveThreshold(consecutive_vec, 3.0, 3), "3 consecutive above 3.0");

  // Edge cases
  std::vector<double> empty_vec;
  EXPECT_TRUE(!hasAtLeastNSamplesAboveThreshold(empty_vec, 1.0, 1), "Empty vector fails N samples test");
  EXPECT_TRUE(hasAtLeastNConsecutiveSamplesAboveThreshold(empty_vec, 1.0, 0), "Empty vector passes 0 consecutive test");

  // Test case 3: Custom condition function
  std::vector<double> condition_vec = {-1.5, 2.5, -0.5, 3.5, 1.5, -2.5, 4.5};

  // Test with lambda for positive values
  auto is_positive = [](const double &x)
  { return x > 0; };
  EXPECT_TRUE(hasAtLeastNSamplesWithConditionTrue(condition_vec, is_positive, 4), "At least 4 positive samples");
  EXPECT_TRUE(!hasAtLeastNSamplesWithConditionTrue(condition_vec, is_positive, 5), "Not 5 positive samples");

  // Test with lambda for even integer values when cast
  auto is_even_when_cast = [](const double &x)
  { return static_cast<int>(x) % 2 == 0; };
  EXPECT_TRUE(hasAtLeastNSamplesWithConditionTrue(condition_vec, is_even_when_cast, 2), "At least 2 even values when cast");

  // Test case 4: Custom condition with consecutive samples
  std::vector<double> consecutive_condition_vec = {0.5, 2.5, 2.8, 2.1, 0.3, 4.1, 4.5, 4.8, 1.2};

  auto above_two = [](const double &x)
  { return x > 2.0; };
  EXPECT_TRUE(hasAtLeastNConsecutiveSamplesWithConditionTrue(consecutive_condition_vec, above_two, 3), "3 consecutive above 2.0");
  EXPECT_TRUE(!hasAtLeastNConsecutiveSamplesWithConditionTrue(consecutive_condition_vec, above_two, 4), "Not 4 consecutive above 2.0");

  auto above_four = [](const double &x)
  { return x > 4.0; };
  EXPECT_TRUE(hasAtLeastNConsecutiveSamplesWithConditionTrue(consecutive_condition_vec, above_four, 3), "3 consecutive above 4.0");
}

void TestBelowThresholdFunctions()
{
  std::vector<double> mixed_vec = {0.5, 1.5, 0.3, 2.1, 2.8, 0.7, 3.2, 3.5, 0.9, 2.9};

  // Test case 1: At least N samples below threshold
  EXPECT_TRUE(hasAtLeastNSamplesBelowThreshold(mixed_vec, 1.0, 3), "At least 3 samples below 1.0");
  EXPECT_TRUE(!hasAtLeastNSamplesBelowThreshold(mixed_vec, 0.4, 2), "Not 2 samples below 0.4");
  EXPECT_TRUE(hasAtLeastNSamplesBelowThreshold(mixed_vec, 2.0, 4), "At least 4 samples below 2.0");

  // Test case 2: Consecutive samples below threshold
  std::vector<double> consecutive_vec = {3.5, 0.2, 0.1, 0.3, 2.8, 0.4, 0.1, 0.2, 3.1, 0.9};

  EXPECT_TRUE(hasAtLeastNConsecutiveSamplesBelowThreshold(consecutive_vec, 1.0, 3), "3 consecutive below 1.0");
  EXPECT_TRUE(!hasAtLeastNConsecutiveSamplesBelowThreshold(consecutive_vec, 0.5, 4), "Not 4 consecutive below 0.5");
  EXPECT_TRUE(hasAtLeastNConsecutiveSamplesBelowThreshold(consecutive_vec, 0.5, 3), "3 consecutive below 0.5");
}

int main()
{
  MyTestMethod();
  TestVarianceAndMeanFunctions();
  TestThresholdFunctions();
  TestBelowThresholdFunctions();

  return 0;
}
