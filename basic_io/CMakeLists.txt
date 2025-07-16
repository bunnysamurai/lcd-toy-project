cmake_minimum_required(VERSION 3.19)

project(basic_io)

option(BUILD_WITH_STDIO_USB "Use pico sdk stdio_usb, instead of stdio to the screen" OFF)

add_library(basic_io 
    bsio.cpp
    TextConsole.cpp
)

set(BASIC_IO_DEPS pico_stdio bsio_screen)

if( BUILD_WITH_STDIO_USB )
    message(STATUS "stdio over USB")
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DBUILD_WITH_STDIO_USB)
    set(BASIC_IO_DEPS ${BASIC_IO_DEPS} pico_stdio_usb)
else()
    message(STATUS "stdio over screen and keyboard")
    add_subdirectory(keyboard)
    set(BASIC_IO_DEPS ${BASIC_IO_DEPS} bsio_keyboard)
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE -O3)
target_include_directories(basic_io PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(basic_io PUBLIC  ${BASIC_IO_DEPS})

add_subdirectory(screen)