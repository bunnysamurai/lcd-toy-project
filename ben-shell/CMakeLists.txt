cmake_minimum_required(VERSION 3.19)

project(ben_shell)

option(BUILD_EXAMPLES "Build examples" OFF)

add_library(${PROJECT_NAME} 
  src/Shell.c
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()