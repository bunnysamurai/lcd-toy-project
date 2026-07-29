cmake_minimum_required(VERSION 3.19)

# application

add_executable(${PROJECT_NAME} 
    main.cpp
    status_utilities.cpp
    common/rng.cpp
    demo.cpp
    snake/snake.cpp
    tetris/tetris.cpp
    revenge/revenge.cpp
    ShellCmd_Menu.cpp
    # chippie :)
    chippie/chippie.cpp
    chippie/state/state.cpp
    chippie/state/game_logic.cpp
    chippie/events/event.cpp
    chippie/entities/basic_entity.cpp
    chippie/entities/moveable_block.cpp
    chippie/entities/chippie_entity.cpp
    chippie/entities/purple_ball.cpp
    chippie/entities/centipede.cpp
    chippie/entities/blue_tank.cpp
    chippie/entities/fire_dancer.cpp
    chippie/entities/water_glider.cpp
    chippie/entities/frog_monster.cpp
    chippie/entities/cyan_stick_ball.cpp
    chippie/entities/bacteria.cpp
    chippie/entities/blob.cpp
    chippie/render/paint_utils.cpp
    chippie/state/chippie_inventory.cpp
    chippie/state/terrain_effects.cpp
    chippie/state/red_brown_buttons.cpp
    chippie/state/teleporter.cpp
    chippie/textures/texture.cpp
    chippie/levels/level.cpp
    chippie/levels/level_1.cpp
    chippie/levels/level_2.cpp
    chippie/levels/level_3.cpp
    chippie/levels/level_4.cpp
    chippie/levels/level_5.cpp
    chippie/levels/level_6.cpp
    chippie/levels/level_7.cpp
    chippie/levels/level_8.cpp
    chippie/levels/level_9.cpp
    chippie/levels/level_10.cpp
    chippie/levels/level_11.cpp
    chippie/levels/level_12.cpp
    chippie/levels/level_13.cpp
    chippie/levels/level_14.cpp
    chippie/levels/level_15.cpp
    chippie/levels/level_16.cpp
    chippie/levels/level_17.cpp
    chippie/levels/level_18.cpp
    chippie/levels/level_19.cpp
    chippie/levels/level_20.cpp
    chippie/levels/level_21.cpp
    chippie/levels/level_22.cpp
    chippie/levels/level_23.cpp
    chippie/levels/level_24.cpp
    chippie/levels/level_25.cpp
    chippie/levels/level_26.cpp
    chippie/levels/level_27.cpp
    chippie/levels/level_28.cpp
    chippie/levels/level_29.cpp
    chippie/levels/level_30.cpp
    chippie/levels/level_31.cpp
    chippie/levels/level_32.cpp
    chippie/levels/level_33.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE -O3)
target_link_options(${PROJECT_NAME} PRIVATE -flto)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_compile_definitions(${PROJECT_NAME} PRIVATE -DCHIPPIE_IS_GIRL)

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