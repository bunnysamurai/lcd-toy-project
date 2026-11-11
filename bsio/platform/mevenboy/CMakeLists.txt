# platform-specific stuff for the MevenBoy


add_library(bsio_platform STATIC platform.cpp)

target_link_libraries(bsio_platform PUBLIC pico_stdio pico_stdlib bsio_includes bsio_utils)
target_include_directories(bsio_platform PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if( STDIO_TO_SCREEN )
    message(STATUS "stdio over screen and keyboard")
    add_subdirectory(keyboard)
    target_link_libraries(bsio_platform PUBLIC bsio_mevenboy_keyboard)
else()
    message(STATUS "stdio over USB")
    target_compile_definitions(bsio_platform PUBLIC -DBUILD_WITH_STDIO_USB)
    target_link_libraries(bsio_platform PUBLIC pico_stdio_usb)
endif()

add_subdirectory(gamepad)
target_link_libraries(bsio_platform PUBLIC bsio_meven_boy_gamepad)

add_subdirectory(screen)
target_link_libraries(bsio_platform PUBLIC waveshare_driver)
