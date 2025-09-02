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
  if (ReferenceDataShouldGenerate())
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

#define TEST_METHOD \
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

  TEST_METHOD
  {
    EXPECT_TRUE(isWithinBounds(x, x_min, x_max), "x is within bounds");
  }

  /*std::vector<double> t, x, x_min, x_max;
  t.resize(N);
  x.resize(N);
  x_min.resize(N);
  x_max.resize(N);

  const double h = 0.01;

  for (int i = 0; i < N; ++i)
  {
    t[i] = static_cast<double>(i) * h;
    x[i] = std::sin(t[i]);
    x_min[i] = x[i] - 0.1;
    x_max[i] = x[i] + 0.1;
  }


  std::vector<double> x_min = GenerateReferenceData("x_trajectory");

  bool result = isWithinBounds(x, x_min, x_max);
  EXPECT_TRUE(result, "Test vector is within bounds");*/
}

int main()
{
  MyTestMethod();

  return 0;
}
