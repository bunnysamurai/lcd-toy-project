
set(KEYBOARD_CONFIG "tinyusb" CACHE STRING "Only 'tinyusb' right meow" FORCE)
set_property(CACHE KEYBOARD_CONFIG PROPERTY STRINGS "tinyusb")

add_library(bsio_keyboard keyboard.cpp)
target_compile_features(bsio_keyboard PRIVATE cxx_std_20)
target_compile_options(bsio_keyboard PRIVATE -O3)
target_link_options(bsio_keyboard PRIVATE -flto)

# here we define our keyboard impl
message("keyboard config is ${KEYBOARD_CONFIG}")
if( KEYBOARD_CONFIG STREQUAL "tinyusb" )
    target_compile_definitions(bsio_keyboard PUBLIC 
        -DTINYUSB_BASICKEYBOARD
    )
    target_link_libraries(bsio_keyboard PRIVATE tinyusb_host)
    target_include_directories(bsio_keyboard PUBLIC ${CMAKE_CURRENT_LIST_DIR}/config)
    target_sources(bsio_keyboard PRIVATE TinyUsbKeyboard.cpp) 
endif()