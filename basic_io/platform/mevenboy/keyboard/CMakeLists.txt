# specific keyboard impl stuff when printing to the screen on the mevenboy

add_library(bsio_mevenboy_keyboard STATIC TinyUsbKeyboard.cpp)
target_compile_features(bsio_mevenboy_keyboard PRIVATE cxx_std_20)
target_compile_options(bsio_mevenboy_keyboard PRIVATE -O3)
target_link_options(bsio_mevenboy_keyboard PRIVATE -flto)

target_link_libraries(bsio_mevenboy_keyboard PRIVATE tinyusb_host bsio_includes)
target_include_directories(bsio_mevenboy_keyboard PUBLIC ${CMAKE_CURRENT_LIST_DIR}/config)
target_include_directories(bsio_mevenboy_keyboard PRIVATE bsio_includes)
target_compile_definitions(bsio_mevenboy_keyboard PUBLIC 
    -DTINYUSB_BASICKEYBOARD
)
