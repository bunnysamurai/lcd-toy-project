cmake_minimum_required(VERSION 3.19)

# application
add_executable(${PROJECT_NAME} 
    main.cpp
    status_utilities.cpp
    # demo.cpp
    snake.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE -O3)
target_link_options(${PROJECT_NAME} PRIVATE -flto)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    pico_stdlib 
    pico_multicore
    pico_rand
    basic_io
    ben_shell
)

pico_add_extra_outputs(${PROJECT_NAME})

# add elf-size-analyze and size -A -d to generate reports

add_custom_command(TARGET ${PROJECT_NAME} 
    POST_BUILD
    COMMAND 
        elf-size-analyze -HaR ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf -W > ram_report.html
)
add_custom_command(TARGET ${PROJECT_NAME} 
    POST_BUILD
    COMMAND 
        elf-size-analyze -HaF ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf -W > ROM_report.html
)
add_custom_command(TARGET ${PROJECT_NAME} 
    POST_BUILD
    COMMAND 
        size -A -d ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf > sections_report.md
)
# add_custom_target(${PROJECT_NAME}_reports ALL DEPENDS )