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
    src/main.cpp
    src/status_utilities.cpp
    basic_io/keyboard/TinyUsbKeyboard.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE -O3)
target_link_options(${PROJECT_NAME} PRIVATE -flto)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    pico_stdlib 
    pico_multicore
    tinyusb_host
    screen_driver
    ben_shell
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src ${CMAKE_CURRENT_LIST_DIR}/basic_io/keyboard)

pico_add_extra_outputs(${PROJECT_NAME})

add_subdirectory(basic_io/screen)
add_subdirectory(ben-shell)
