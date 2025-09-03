#pragma once

#include <vector>
#include <type_traits>
#include <stdexcept>
#include <numeric>
#include <cmath>
#include <functional>

#include <duoplot/duoplot.h>
namespace lumos
{

  template <typename T>
  T linearInterpolate(T x, T x0, T y0, T x1, T y1)
  {
    static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>,
                  "linearInterpolate only supports float and double types");
    if (x1 == x0)
      return y0;
    return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
  }

  template <typename T>
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

    if (target_time <= time_vec[0])
      return value_vec[0];
    if (target_time >= time_vec.back())
      return value_vec.back();

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

  template <typename T>
  bool isWithinBounds(const std::vector<T> &test_vector,
                      const std::vector<T> &min_bounds,
                      const std::vector<T> &max_bounds)
  {
    static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>,
                  "isWithinBounds only supports float and double types");
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

  template <typename T>
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

  template <typename T>
  bool isVarianceWithinThreshold(const std::vector<T> &test_vector,
                                 const std::vector<T> &reference_vector,
                                 T threshold)
  {
    static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>,
                  "isVarianceWithinThreshold only supports float and double types");

    if (test_vector.size() != reference_vector.size())
    {
      return false;
    }

    if (test_vector.empty())
    {
      return true;
    }

    // Calculate variance of test vector relative to reference
    T sum_squared_diff = T(0);
    for (size_t i = 0; i < test_vector.size(); ++i)
    {
      T diff = test_vector[i] - reference_vector[i];
      sum_squared_diff += diff * diff;
    }

    T variance = sum_squared_diff / static_cast<T>(test_vector.size());
    return variance <= threshold;
  }

  template <typename T>
  bool isMeanDifferenceWithinThreshold(const std::vector<T> &test_vector,
                                       const std::vector<T> &reference_vector,
                                       T threshold)
  {
    static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>,
                  "isMeanDifferenceWithinThreshold only supports float and double types");

    if (test_vector.size() != reference_vector.size())
    {
      return false;
    }

    if (test_vector.empty())
    {
      return true;
    }

    T test_mean = std::accumulate(test_vector.begin(), test_vector.end(), T(0)) /
                  static_cast<T>(test_vector.size());
    T ref_mean = std::accumulate(reference_vector.begin(), reference_vector.end(), T(0)) /
                 static_cast<T>(reference_vector.size());

    T mean_diff = std::abs(test_mean - ref_mean);
    return mean_diff <= threshold;
  }

  template <typename T>
  bool hasAtLeastNSamplesAboveThreshold(const std::vector<T> &test_vector,
                                        T threshold,
                                        size_t min_samples)
  {
    static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>,
                  "hasAtLeastNSamplesAboveThreshold only supports float and double types");

    size_t count = 0;
    for (const T &value : test_vector)
    {
      if (value > threshold)
      {
        count++;
      }
    }

    return count >= min_samples;
  }

  template <typename T>
  bool hasAtLeastNConsecutiveSamplesAboveThreshold(const std::vector<T> &test_vector,
                                                   T threshold,
                                                   size_t min_consecutive)
  {
    static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>,
                  "hasAtLeastNConsecutiveSamplesAboveThreshold only supports float and double types");

    if (min_consecutive == 0)
    {
      return true;
    }

    size_t consecutive_count = 0;
    for (const T &value : test_vector)
    {
      if (value > threshold)
      {
        consecutive_count++;
        if (consecutive_count >= min_consecutive)
        {
          return true;
        }
      }
      else
      {
        consecutive_count = 0;
      }
    }

    return false;
  }

  template <typename T>
  bool hasAtLeastNSamplesBelowThreshold(const std::vector<T> &test_vector,
                                        T threshold,
                                        size_t min_samples)
  {
    static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>,
                  "hasAtLeastNSamplesBelowThreshold only supports float and double types");

    size_t count = 0;
    for (const T &value : test_vector)
    {
      if (value < threshold)
      {
        count++;
      }
    }

    return count >= min_samples;
  }

  template <typename T>
  bool hasAtLeastNConsecutiveSamplesBelowThreshold(const std::vector<T> &test_vector,
                                                   T threshold,
                                                   size_t min_consecutive)
  {
    static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>,
                  "hasAtLeastNConsecutiveSamplesBelowThreshold only supports float and double types");

    if (min_consecutive == 0)
    {
      return true;
    }

    size_t consecutive_count = 0;
    for (const T &value : test_vector)
    {
      if (value < threshold)
      {
        consecutive_count++;
        if (consecutive_count >= min_consecutive)
        {
          return true;
        }
      }
      else
      {
        consecutive_count = 0;
      }
    }

    return false;
  }

  template <typename T, typename Predicate>
  bool hasAtLeastNSamplesWithConditionTrue(const std::vector<T> &test_vector,
                                           Predicate condition,
                                           size_t min_samples)
  {
    static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>,
                  "hasAtLeastNSamplesWithConditionTrue only supports float and double types");

    size_t count = 0;
    for (const T &value : test_vector)
    {
      if (condition(value))
      {
        count++;
      }
    }

    return count >= min_samples;
  }

  template <typename T, typename Predicate>
  bool hasAtLeastNConsecutiveSamplesWithConditionTrue(const std::vector<T> &test_vector,
                                                      Predicate condition,
                                                      size_t min_consecutive)
  {
    static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>,
                  "hasAtLeastNConsecutiveSamplesWithConditionTrue only supports float and double types");

    if (min_consecutive == 0)
    {
      return true;
    }

    size_t consecutive_count = 0;
    for (const T &value : test_vector)
    {
      if (condition(value))
      {
        consecutive_count++;
        if (consecutive_count >= min_consecutive)
        {
          return true;
        }
      }
      else
      {
        consecutive_count = 0;
      }
    }

    return false;
  }

  template <typename T>
  bool isWithin2DCorridor(
      const std::vector<T> &x_test, const std::vector<T> &y_test,
      const std::vector<T> &x_left, const std::vector<T> &y_left,
      const std::vector<T> &x_right, const std::vector<T> &y_right)
  {
    static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>,
                  "isWithin2DCorridor only supports float and double types");

    if (x_test.size() != y_test.size())
    {
      throw std::invalid_argument("Test vectors must have the same size");
    }

    if (x_left.size() != y_left.size() || x_right.size() != y_right.size())
    {
      throw std::invalid_argument("Boundary vectors must have consistent sizes");
    }

    if (x_left.size() < 2 || x_right.size() < 2)
    {
      throw std::invalid_argument("Boundary vectors must have at least 2 points");
    }

    // Helper function to check if point is on the left side of a line segment
    auto isPointOnLeftSide = [](T px, T py, T x1, T y1, T x2, T y2) -> bool
    {
      return ((x2 - x1) * (py - y1) - (y2 - y1) * (px - x1)) >= 0;
    };

    for (size_t i = 0; i < x_test.size(); ++i)
    {
      T test_x = x_test[i];
      T test_y = y_test[i];
      bool inside_corridor = true;

      // Check if point is inside the corridor by verifying it's on the correct side
      // of all left boundary segments and correct side of all right boundary segments

      // Check left boundary - point should be on the right side of left boundary
      for (size_t j = 0; j < x_left.size() - 1; ++j)
      {
        if (isPointOnLeftSide(test_x, test_y, x_left[j], y_left[j], x_left[j + 1], y_left[j + 1]))
        {
          inside_corridor = false;
          break;
        }
      }

      if (!inside_corridor)
        continue;

      // Check right boundary - point should be on the left side of right boundary
      for (size_t j = 0; j < x_right.size() - 1; ++j)
      {
        if (!isPointOnLeftSide(test_x, test_y, x_right[j], y_right[j], x_right[j + 1], y_right[j + 1]))
        {
          inside_corridor = false;
          break;
        }
      }

      if (!inside_corridor)
      {
        return false;
      }
    }

    return true;
  }

}
