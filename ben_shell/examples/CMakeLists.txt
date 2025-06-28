
add_executable(${PROJECT_NAME}_example main.c)
target_link_libraries(${PROJECT_NAME}_example PRIVATE ${PROJECT_NAME})