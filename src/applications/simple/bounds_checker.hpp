#pragma once

#include <vector>
#include <type_traits>
#include <stdexcept>

template<typename T>
T linearInterpolate(T x, T x0, T y0, T x1, T y1)
{
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>, 
                "linearInterpolate only supports float and double types");
  if (x1 == x0) return y0;
  return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
}

template<typename T>
T interpolateAtTime(T target_time, 
                   const std::vector<T> &time_vec, 
                   const std::vector<T> &value_vec)
{
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>, 
                "interpolateAtTime only supports float and double types");
  if (time_vec.size() != value_vec.size() || time_vec.empty())
  {
    throw std::invalid_argument("Time and value vectors must have same non-zero size");
  }
  
  if (target_time <= time_vec[0]) return value_vec[0];
  if (target_time >= time_vec.back()) return value_vec.back();
  
  for (size_t i = 0; i < time_vec.size() - 1; ++i)
  {
    if (target_time >= time_vec[i] && target_time <= time_vec[i + 1])
    {
      return linearInterpolate(target_time, time_vec[i], value_vec[i], 
                              time_vec[i + 1], value_vec[i + 1]);
    }
  }
  
  return value_vec.back();
}

bool isWithinBounds(const std::vector<double> &test_vector,
                    const std::vector<double> &min_bounds,
                    const std::vector<double> &max_bounds)
{
  if (test_vector.size() != min_bounds.size() || test_vector.size() != max_bounds.size())
  {
    return false;
  }
  
  for (size_t i = 0; i < test_vector.size(); ++i)
  {
    if (test_vector[i] < min_bounds[i] || test_vector[i] > max_bounds[i])
    {
      return false;
    }
  }
  
  return true;
}

template<typename T>
bool isWithinBounds(const std::vector<T> &test_vector_time,
                    const std::vector<T> &test_vector,
                    const std::vector<T> &min_bounds_time,
                    const std::vector<T> &min_bounds,
                    const std::vector<T> &max_bounds_time,
                    const std::vector<T> &max_bounds)
{
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>, 
                "isWithinBounds only supports float and double types");
  if (test_vector_time.size() != test_vector.size())
  {
    return false;
  }
  
  if (min_bounds_time.size() != min_bounds.size() || 
      max_bounds_time.size() != max_bounds.size())
  {
    return false;
  }
  
  for (size_t i = 0; i < test_vector.size(); ++i)
  {
    T time = test_vector_time[i];
    T test_value = test_vector[i];
    
    T min_bound = interpolateAtTime(time, min_bounds_time, min_bounds);
    T max_bound = interpolateAtTime(time, max_bounds_time, max_bounds);
    
    if (test_value < min_bound || test_value > max_bound)
    {
      return false;
    }
  }
  
  return true;
}