cmake_minimum_required(VERSION 3.19)
project(SDL2Test)

include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_BRANCH master
)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(OpenGL REQUIRED)
find_package(fmt REQUIRED)
FetchContent_MakeAvailable(imgui)

file(GLOB IMGUI_CORE_SRC ${imgui_SOURCE_DIR}/*.cpp )
add_executable(${PROJECT_NAME}_imgui
    imgui_main.cpp 
    ${IMGUI_CORE_SRC}
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
)
target_link_libraries(${PROJECT_NAME}_imgui PRIVATE ${SDL2_LIBRARIES} OpenGL::OpenGL fmt)
target_include_directories(${PROJECT_NAME}_imgui PRIVATE ${imgui_SOURCE_DIR})

add_executable(${PROJECT_NAME}_sdl
    sdl_main.cpp 
)
target_link_libraries(${PROJECT_NAME}_sdl PRIVATE ${SDL2_LIBRARIES} fmt)
