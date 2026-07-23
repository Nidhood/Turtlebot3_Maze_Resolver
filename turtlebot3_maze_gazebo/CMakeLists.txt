cmake_minimum_required(VERSION 3.8)

project(
    turtlebot3_maze_gazebo
    VERSION 1.0.0
    DESCRIPTION "Gazebo spawner for the TurtleBot3 school maze"
    LANGUAGES CXX
)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror=return-type
    )
endif()

find_package(ament_cmake REQUIRED)

install(
    DIRECTORY
        launch
        models
    DESTINATION
        share/${PROJECT_NAME}/
)

ament_package()
