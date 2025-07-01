cmake_minimum_required(VERSION 3.14)

include(add_unit_test.cmake)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_gtest_executable(variable_array_test)
add_gtest_executable(circular_array_test)

add_unit_test(variable_array variable_array_test)
add_unit_test(circular_array circular_array_test)
run_all_tests()