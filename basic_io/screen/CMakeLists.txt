
project(bsio_screen)

add_library(${PROJECT_NAME} STATIC 
    screen.cpp
    tile_blitting.cpp
    glyphs/letter_utils.cpp
    gfx/shapes.cpp
    gfx/dialog.cpp
    gfx/cursor_menu_dialog.cpp
    gfx/number_dialog.cpp
    gfx/details/dialog_utils.cpp
)
target_compile_options(${PROJECT_NAME} PUBLIC -O3)
target_link_options(${PROJECT_NAME} PUBLIC -flto)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
# target_compile_definitions(${PROJECT_NAME} PUBLIC -DRUN_CONSTEXPR_TESTS_IN_STATIC_ASSERTS)
target_link_libraries(${PROJECT_NAME} PUBLIC embp bsio_platform)

if( BUILD_TESTS )

# TODO take note here, this might be a good pattern to wrap a cmake function around
# add_executable(dialog.test gfx/dialog.test.cc)
# target_link_libraries(dialog.test PRIVATE catch_tests ${PROJECT_NAME})
# add_unit_test(dialog.test dialog.test)

add_executable(string_utils.test details/string_utils.test.cc)
target_link_libraries(string_utils.test PRIVATE catch_tests ${PROJECT_NAME})
add_unit_test(string_utils.test string_utils.test)

endif()