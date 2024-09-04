
project(screen_driver)

option(BUILD_FOR_WAVESHARE "build for waveshare lcd" ON)

if( BUILD_FOR_WAVESHARE )
    add_subdirectory(waveshare_driver)
    set(${PROJECT_NAME}_SOURCES "waveshare_driver/display_impl.cpp")
    set(${PROJECT_NAME}_LIBS waveshare_driver)
endif()

add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC -O3)
target_link_options(${PROJECT_NAME} PUBLIC -flto)

target_link_libraries(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_LIBS})