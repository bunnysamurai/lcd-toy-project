
project(screen_driver)


add_subdirectory(waveshare_driver)
set(${PROJECT_NAME}_SOURCES "display.cpp")
set(${PROJECT_NAME}_LIBS waveshare_driver)

add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC -O3)
target_link_options(${PROJECT_NAME} PUBLIC -flto)

target_link_libraries(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_LIBS})