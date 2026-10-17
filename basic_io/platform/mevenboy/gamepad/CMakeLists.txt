
project(bsio_meven_boy_gamepad)

add_library(${PROJECT_NAME} STATIC gamepad_mevenboy.cpp)
target_compile_options(${PROJECT_NAME} PUBLIC -O3)
target_link_options(${PROJECT_NAME} PUBLIC -flto)
target_include_directories(${PROJECT_NAME} PRIVATE bsio_includes)
target_link_libraries(${PROJECT_NAME} PUBLIC pico_time hardware_gpio bsio_includes)
