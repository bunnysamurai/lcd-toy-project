cmake_minimum_required(VERSION 3.14)

project(embp)

option(BUILD_TESTS "Configure and build tests" OFF)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_compile_options(${PROJECT_NAME} INTERFACE -O3)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
target_compile_definitions(${PROJECT_NAME} INTERFACE -DSTD_LIB_AVAILABLE)

if( BUILD_TESTS )
    include(CTest)
    add_subdirectory(tests)
endif()