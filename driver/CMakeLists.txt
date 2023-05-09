
set(PICO_SDK_PATH /home/bunnysamurai/sw/pico-sdk)
# boilerplate, always before project()
include(../cmake/pico_sdk_import.cmake)

project(waveshare_driver)

# boilerplate, always after project()
pico_sdk_init()

add_library(waveshare_driver STATIC dispWaveshareLcd.c)
target_compile_options(${PROJECT_NAME} PRIVATE -O3)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    pico_stdlib 
    hardware_dma 
    hardware_pio 
    hardware_gpio 
    hardware_resets
    hardware_irq
    pico_printf
)

