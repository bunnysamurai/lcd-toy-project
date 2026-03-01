
project(lcd_toy_tests)

include(CTest)
include(../cmake/add_unit_test.cmake)

add_gtest_executable(tile_blitting)
target_compile_features(tile_blitting PUBLIC cxx_std_20)
target_include_directories(tile_blitting PUBLIC ../basic_io)
target_link_libraries(tile_blitting PUBLIC bsio_screen)
add_unit_test(tile_blitting tile_blitting)

add_gtest_executable(dummy)
add_unit_test(dummy dummy)

run_all_tests()