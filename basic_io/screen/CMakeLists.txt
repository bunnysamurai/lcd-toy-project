
project(bsio_screen)


add_subdirectory(waveshare_driver)
set(${PROJECT_NAME}_SOURCES "screen.cpp")
set(${PROJECT_NAME}_LIBS waveshare_driver)

add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC -O3)
target_link_options(${PROJECT_NAME} PUBLIC -flto)

target_link_libraries(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_LIBS})