
add_library(bsio_console STATIC
    TextConsole.cpp
    details/tile_blitting.cpp
)

target_compile_features(bsio_console PUBLIC cxx_std_20)
target_compile_options(bsio_console PUBLIC -O3)
target_link_options(bsio_console PUBLIC -flto)
target_link_libraries(bsio_console PUBLIC pico_time)