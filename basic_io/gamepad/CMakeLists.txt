
#set(PICO_SDK_PATH /home/bunnysamurai/sw/pico-sdk)
# boilerplate, always before project()
# message(${CMAKE_SOURCE_DIR}/cmake/pico_sdk_import.cmake)
# include(${CMAKE_SOURCE_DIR}/cmake/pico_sdk_import.cmake)

project(bsio_gamepad)

# boilerplate, always after project()
# pico_sdk_init()

add_library(${PROJECT_NAME} STATIC gamepad.cpp)
target_compile_options(${PROJECT_NAME} PUBLIC -O3)
target_link_options(${PROJECT_NAME} PUBLIC -flto)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    pico_time
    hardware_gpio 
)
