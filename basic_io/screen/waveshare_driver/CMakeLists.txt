
#set(PICO_SDK_PATH /home/bunnysamurai/sw/pico-sdk)
# boilerplate, always before project()
# message(${CMAKE_SOURCE_DIR}/cmake/pico_sdk_import.cmake)
# include(${CMAKE_SOURCE_DIR}/cmake/pico_sdk_import.cmake)

project(waveshare_driver)

# boilerplate, always after project()
# pico_sdk_init()

add_library(${PROJECT_NAME}_clib STATIC dispWaveshareLcd.c)
target_compile_options(${PROJECT_NAME}_clib PUBLIC -O3)
target_link_options(${PROJECT_NAME}_clib PUBLIC -flto)

target_link_libraries(${PROJECT_NAME}_clib PUBLIC 
    pico_stdlib 
    hardware_dma 
    hardware_pio 
    hardware_gpio 
    hardware_resets
    hardware_irq
    pico_printf
)

add_library(${PROJECT_NAME} STATIC screen_impl.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${PROJECT_NAME}_clib
    embp
)

    