cmake_minimum_required(VERSION 3.19)

option( BUILD_TESTS "build tests instead" OFF)

if( NOT BUILD_TESTS )

# per the instructions on the [github README](https://github.com/raspberrypi/pico-sdk) for automatic download from GitHub. None of this manual rubbish.
set(PICO_SDK_PATH /home/bunnysamurai/sw/pico-sdk)
# boilerplate, always before project()
include(cmake/pico_sdk_import.cmake)

endif()

project(lcd_toy_project)

if ( BUILD_TESTS )


# add unit tests by simply adding the sources-under-test and their test files to the target "tests"
# be sure to also include the mocks
message(STATUS "Building tests")

find_package(Catch2 3 REQUIRED)

add_executable(tests)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

add_subdirectory(basic_io)
add_subdirectory(embp) # required for some tests to build

include(CTest)
include(Catch)
catch_discover_tests(tests)

else()

# boilerplate, always after project()
pico_sdk_init()

add_subdirectory(basic_io)
add_subdirectory(ben_shell)
add_subdirectory(src)
add_subdirectory(embp)

endif()