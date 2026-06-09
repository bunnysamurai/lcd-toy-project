
project(bsio_screen)

#set(SCREEN_CONFIG "waveshare240p" CACHE STRING "Only 'waveshare240p' right meow" FORCE)
set_property(CACHE SCREEN_CONFIG PROPERTY STRINGS "waveshare240p" "dummy")

add_library(${PROJECT_NAME} STATIC 
    screen.cpp
    tile_blitting.cpp
    glyphs/letter_utils.cpp
    gfx/shapes.cpp
    gfx/dialog.cpp
)
target_compile_options(${PROJECT_NAME} PUBLIC -O3)
target_link_options(${PROJECT_NAME} PUBLIC -flto)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_link_libraries(${PROJECT_NAME} PUBLIC 
    embp
    pico_stdlib
)

message(STATUS "Screen configured for ${SCREEN_CONFIG}")

if( SCREEN_CONFIG STREQUAL "waveshare240p" )
    message("Adding waveshare_driver")
    add_subdirectory(waveshare_driver)
    target_link_libraries(${PROJECT_NAME} PUBLIC waveshare_driver)

    # here we define the capabilities of our SPI screen
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        -DWAVESHARE_240P
        -DMAX_SUPPORTED_BPP=16
    )
    target_compile_definitions(waveshare_driver_clib PUBLIC
        -DWAVESHARE_240P
        -DMAX_SUPPORTED_BPP=16
    )
elseif( SCREEN_CONFIG STREQUAL "dummy" )
    message("Adding dummy_driver")
    add_subdirectory(dummy_driver)
    target_link_libraries(${PROJECT_NAME} PUBLIC dummy_driver)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        -DDUMMY_SCREEN
        -DMAX_SUPPORTED_BPP=16
    )
endif()
