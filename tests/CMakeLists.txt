cmake_minimum_required(VERSION 3.19)

project(lcd_toy_tests)
add_executable(${PROJECT_NAME}
    tile_blitting.cc
    ../basic_io/screen/tile_blitting.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ../basic_io/screen)
