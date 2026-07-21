
project(bsio_screen)

set_property(CACHE SCREEN_CONFIG PROPERTY STRINGS "waveshare240p" "dummy")

add_library(${PROJECT_NAME} STATIC 
    screen.cpp
    tile_blitting.cpp
    glyphs/letter_utils.cpp
    gfx/shapes.cpp
    gfx/dialog.cpp
    gfx/cursor_menu_dialog.cpp
)
target_compile_options(${PROJECT_NAME} PUBLIC -O3)
target_link_options(${PROJECT_NAME} PUBLIC -flto)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
# target_compile_definitions(${PROJECT_NAME} PUBLIC -DRUN_CONSTEXPR_TESTS_IN_STATIC_ASSERTS)
target_link_libraries(${PROJECT_NAME} PUBLIC embp)
if( CMAKE_CROSSCOMPILING )
target_link_libraries(${PROJECT_NAME} PUBLIC pico_stdlib)
endif()

message(STATUS "Screen configured for ${SCREEN_CONFIG}")

if( SCREEN_CONFIG STREQUAL "waveshare240p" AND CMAKE_CROSSCOMPILING )
    message("Adding waveshare_driver")
    add_subdirectory(waveshare_driver)
    target_link_libraries(${PROJECT_NAME} PUBLIC waveshare_driver)

    # here we define the capabilities of our SPI screen
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        -DWAVESHARE_240P
        -DMAX_SUPPORTED_BPP=16
    )
    target_compile_definitions(waveshare_driver_clib PUBLIC
        -DWAVESHARE_240P
        -DMAX_SUPPORTED_BPP=16
    )
elseif( SCREEN_CONFIG STREQUAL "dummy" OR NOT CMAKE_CROSSCOMPILING )
    message("Adding dummy_driver")
    add_subdirectory(dummy_driver)
    target_link_libraries(${PROJECT_NAME} PUBLIC dummy_driver)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        -DDUMMY_SCREEN
        -DMAX_SUPPORTED_BPP=16
    )
endif()

if( NOT CMAKE_CROSSCOMPILING )

# TODO take note here, this might be a good pattern to wrap a cmake function around
# add_executable(dialog.test gfx/dialog.test.cc)
# target_link_libraries(dialog.test PRIVATE catch_tests ${PROJECT_NAME})
# add_unit_test(dialog.test dialog.test)

add_executable(string_utils.test details/string_utils.test.cc)
target_link_libraries(string_utils.test PRIVATE catch_tests ${PROJECT_NAME})
add_unit_test(string_utils.test string_utils.test)

endif()