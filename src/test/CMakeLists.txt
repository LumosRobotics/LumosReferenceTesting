cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# Original comprehensive test
set(CPP_SOURCE_FILES test.cpp)
add_executable(test_runner ${CPP_SOURCE_FILES})
target_link_libraries(test_runner ${GTEST_LIB_FILES})
add_test(NAME bounds_checker_tests COMMAND test_runner)

# Individual function tests
add_executable(test_linear_interpolate test_linear_interpolate.cpp)
target_link_libraries(test_linear_interpolate ${GTEST_LIB_FILES})
add_test(NAME linear_interpolate_tests COMMAND test_linear_interpolate)

add_executable(test_interpolate_at_time test_interpolate_at_time.cpp)
target_link_libraries(test_interpolate_at_time ${GTEST_LIB_FILES})
add_test(NAME interpolate_at_time_tests COMMAND test_interpolate_at_time)

add_executable(test_is_within_bounds test_is_within_bounds.cpp)
target_link_libraries(test_is_within_bounds ${GTEST_LIB_FILES})
add_test(NAME is_within_bounds_tests COMMAND test_is_within_bounds)

add_executable(test_variance_threshold test_variance_threshold.cpp)
target_link_libraries(test_variance_threshold ${GTEST_LIB_FILES})
add_test(NAME variance_threshold_tests COMMAND test_variance_threshold)

add_executable(test_mean_difference test_mean_difference.cpp)
target_link_libraries(test_mean_difference ${GTEST_LIB_FILES})
add_test(NAME mean_difference_tests COMMAND test_mean_difference)

add_executable(test_threshold_functions test_threshold_functions.cpp)
target_link_libraries(test_threshold_functions ${GTEST_LIB_FILES})
add_test(NAME threshold_functions_tests COMMAND test_threshold_functions)

add_executable(test_condition_functions test_condition_functions.cpp)
target_link_libraries(test_condition_functions ${GTEST_LIB_FILES})
add_test(NAME condition_functions_tests COMMAND test_condition_functions)

add_executable(test_2d_corridor test_2d_corridor.cpp)
target_link_libraries(test_2d_corridor ${GTEST_LIB_FILES})
add_test(NAME 2d_corridor_tests COMMAND test_2d_corridor)

add_executable(application_test application_test.cpp)
target_link_libraries(application_test ${GTEST_LIB_FILES})
add_test(NAME application_tests COMMAND application_test)