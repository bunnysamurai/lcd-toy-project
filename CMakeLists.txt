cmake_minimum_required(VERSION 3.19)

option( BUILD_TESTS "build tests instead" OFF)

if ( BUILD_TESTS )
message("building tests")
add_subdirectory(tests)
add_subdirectory(basic_io/screen)
add_subdirectory(embp)
else()
# per the instructions on the [github README](https://github.com/raspberrypi/pico-sdk) for automatic download from GitHub. None of this manual rubbish.
set(PICO_SDK_PATH /home/bunnysamurai/sw/pico-sdk)
# boilerplate, always before project()
include(cmake/pico_sdk_import.cmake)

project(lcd_toy_project)

# boilerplate, always after project()
pico_sdk_init()

add_subdirectory(basic_io)
add_subdirectory(ben_shell)
add_subdirectory(src)
add_subdirectory(embp)
endif()