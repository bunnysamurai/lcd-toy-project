cmake_minimum_required(VERSION 3.19)

project(basic_io)

add_library(basic_io 
    bsio.cpp
    console/details/tile_blitting.cpp
)
target_link_libraries(basic_io PUBLIC 
    bsio_screen
    bsio_keyboard
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE -O3)
target_include_directories(basic_io PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(keyboard)
add_subdirectory(screen)