cmake_minimum_required(VERSION 3.16)

# per the instructions on the [github README](https://github.com/raspberrypi/pico-sdk) for automatic download from GitHub. None of this manual rubbish.
set(PICO_SDK_FETCH_FROM_GIT on)
# boilerplate, always before project()
include(cmake/pico_sdk_import.cmake)

project(lcd-toy-project)

# boilerplate, always after project()
pico_sdk_init()

add_executable(${PROJECT_NAME} src/main_blink.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE pico_stdlib)

pico_add_extra_outputs(${PROJECT_NAME})