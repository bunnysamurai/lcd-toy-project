cmake_minimum_required(VERSION 3.19)

# application
add_executable(${PROJECT_NAME} 
    main.cpp
    status_utilities.cpp
    demo.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE -O3)
target_link_options(${PROJECT_NAME} PRIVATE -flto)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    pico_stdlib 
    pico_multicore
    basic_io
    ben_shell
)

pico_add_extra_outputs(${PROJECT_NAME})