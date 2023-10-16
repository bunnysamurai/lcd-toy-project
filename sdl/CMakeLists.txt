cmake_minimum_required(VERSION 3.19)
project(SDL2Test)

find_package(SDL2 REQUIRED)
find_package(fmt REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES} fmt)
