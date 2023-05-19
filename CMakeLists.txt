cmake_minimum_required(VERSION 3.16)

# per the instructions on the [github README](https://github.com/raspberrypi/pico-sdk) for automatic download from GitHub. None of this manual rubbish.
set(PICO_SDK_PATH /home/bunnysamurai/sw/pico-sdk)
# boilerplate, always before project()
include(cmake/pico_sdk_import.cmake)

project(lcd-toy-project)

# boilerplate, always after project()
pico_sdk_init()

#app
add_executable(${PROJECT_NAME} 
    src/main_blink.cpp
    src/status_utilities.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE -O3)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

target_link_libraries(${PROJECT_NAME} PRIVATE pico_stdlib waveshare_driver pico_rand)

# so printf works?
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})

add_subdirectory(driver)
