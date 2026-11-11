
project(bsio_utils)

add_library(${PROJECT_NAME} STATIC 
    TextConsole.cpp
)
target_compile_options(${PROJECT_NAME} PUBLIC -O3)
target_link_options(${PROJECT_NAME} PUBLIC -flto)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_link_libraries(${PROJECT_NAME} PUBLIC bsio_includes bsio_screen)
