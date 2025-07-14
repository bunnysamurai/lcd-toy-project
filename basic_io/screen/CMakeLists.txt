
project(bsio_screen)

set(SCREEN_CONFIG "waveshare240p" CACHE STRING "Only 'waveshare240p' right meow" FORCE)
set_property(CACHE SCREEN_CONFIG PROPERTY STRINGS "waveshare240p")

add_library(${PROJECT_NAME} STATIC 
    screen.cpp
    tile_blitting.cpp
)
target_compile_options(${PROJECT_NAME} PUBLIC -O3)
target_link_options(${PROJECT_NAME} PUBLIC -flto)
target_link_libraries(${PROJECT_NAME} PUBLIC 
    pico_time
    embp
)

if( SCREEN_CONFIG STREQUAL "waveshare240p" )
    add_subdirectory(waveshare_driver)
    target_link_libraries(${PROJECT_NAME} PUBLIC waveshare_driver)

    # here we define the capabilities of our SPI screen
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        -DWAVESHARE_240P
        -DMAX_SUPPORTED_BPP=16
    )
    target_compile_definitions(waveshare_driver PUBLIC
        -DWAVESHARE_240P
        -DMAX_SUPPORTED_BPP=16
    )
endif()