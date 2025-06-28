
add_library(bsio_keyboard TinyUsbKeyboard.cpp)
target_compile_features(bsio_keyboard PRIVATE cxx_std_20)
target_compile_options(bsio_keyboard PRIVATE -O3)
target_link_options(bsio_keyboard PRIVATE -flto)
target_link_libraries(bsio_keyboard PRIVATE tinyusb_host)
target_include_directories(bsio_keyboard PUBLIC ${CMAKE_CURRENT_LIST_DIR}/config)