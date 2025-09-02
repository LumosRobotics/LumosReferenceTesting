#include <iostream>
#include <vector>
// #include <nlohmann/json.hpp>

int main()
{
  constexpr int N = 100;
  std::vector<double> t, x, x_min, x_max;
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

  return 0;
}
