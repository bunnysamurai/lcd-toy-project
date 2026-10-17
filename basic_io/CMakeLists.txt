cmake_minimum_required(VERSION 4.00)

project(basic_io)

if( NOT BUILD_TESTS )

set_property(CACHE PLATFORM_CONFIG PROPERTY STRINGS "mevenboy" "desktop")
option(STDIO_TO_SCREEN "stdio redirects to screen instead of default" OFF)

add_library(bsio_includes INTERFACE)
target_include_directories(bsio_includes INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_library(basic_io 
    bsio.cpp
    TextConsole.cpp
)

# pull in platform-specific interface lib, which will define bsio_platform target
if( PLATFORM_CONFIG STREQUAL "mevenboy" )
    message(STATUS "Platform: mevenboy")
    add_subdirectory(platform/mevenboy)
elseif( PLATFORM_CONFIG STREQUAL "desktop" )
    message(STATUS "Platform: desktop")
    add_subdirectory(platform/desktop)
else()
    message(FATAL_ERROR "unreachable... there's a bug")
endif()

add_subdirectory(screen)
add_subdirectory(gamepad)
add_subdirectory(keyboard)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE -O3)
target_link_libraries(basic_io PUBLIC bsio_includes bsio_screen bsio_gamepad bsio_keyboard bsio_platform)


else()

message(STATUS "Building tests for ${PROJECT_NAME}")

add_subdirectory(screen)


endif()