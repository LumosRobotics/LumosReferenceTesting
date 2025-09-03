#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <utility>
// #include <nlohmann/json.hpp>
#include "reference_testing/reference_testing.h"

using namespace lumos;

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

constexpr int N = 1000;

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
  using namespace duoplot;

  Vector<double> x_d = x;
  Vector<double> t_d = t;

  Vector<double> x_min_d = x_min;
  Vector<double> x_max_d = x_max;
  Vector<double> x_ref_d = x_ref;

  setCurrentElement("p_view_0");
  clearView();
  axis({0.0, -1.0, -1.0}, {5.0, 5.0, 1.0});

  plot(t_d, x_d);
  plot(t_d, x_min_d);
  plot(t_d, x_max_d);
  plot(t_d, x_ref_d);

  /*TEST_METHOD("Some name for dvs window")
  {
    EXPECT_TRUE(isWithinBounds(x, x_min, x_max), "x is within bounds");
    EXPECT_TRUE(isVarianceWithinThreshold(x, x_ref, 0.01), "x variance within threshold");
    EXPECT_TRUE(isMeanDifferenceWithinThreshold(x, x_ref, 0.05), "x mean difference within threshold");
  }*/
}

int main()
{
  MyTestMethod();

  return 0;
}
