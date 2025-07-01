function(add_unit_test target target_test )
    set(UNIT_TEST_TARGETS ${UNIT_TEST_TARGETS} ${target_test} PARENT_SCOPE)
    add_test(NAME ${target} COMMAND ${target_test})
endfunction(add_unit_test)

function(add_gtest_executable target)
    add_executable(${target} ${target}.cc)
    target_compile_options(${target} PRIVATE -fpic -O3)
    set_target_properties(${target} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
    target_link_libraries(${target} PRIVATE gtest gtest_main)
endfunction(add_gtest_executable)

function(run_all_tests)
    add_custom_target( all_tests ALL
        DEPENDS ${UNIT_TEST_TARGETS}
        )

    add_custom_command(TARGET all_tests
        COMMENT "Run tests"
        POST_BUILD COMMAND ctest ARGS --output-on-failure
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
        )
endfunction()

