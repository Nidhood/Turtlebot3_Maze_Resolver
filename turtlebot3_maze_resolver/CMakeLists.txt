cmake_minimum_required(VERSION 3.8)

# Project initial configuration:
project(
    turtlebot3_maze_resolver
    VERSION 0.0.1
    DESCRIPTION "Cell-based maze execution system for TurtleBot3"
    LANGUAGES CXX
)

# C++ basic configuration:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror=return-type
    )
endif()

# Qt configuration:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ROS2 dependencies:
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(
    Qt6 REQUIRED
    COMPONENTS
        Core
        Widgets
)

# C++ motion core:
add_library(
    ${PROJECT_NAME}_core
    STATIC
    src/cell_motion_controller.cpp
    src/core/services/maze_session.cpp
)

target_include_directories(
    ${PROJECT_NAME}_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# ROS2 node configuration:
add_executable(
    cell_motion_node
    src/cell_motion_node.cpp
    src/main.cpp
)

target_include_directories(
    cell_motion_node
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(
    cell_motion_node
    ${PROJECT_NAME}_core
)

ament_target_dependencies(
    cell_motion_node
    rclcpp
    std_msgs
    geometry_msgs
    nav_msgs
)

# GUI headers:
set(GUI_HEADERS

    include/turtlebot3_maze_resolver/app/app_context.hpp
    include/turtlebot3_maze_resolver/app/main_window.hpp
    include/turtlebot3_maze_resolver/app/maze_execution_controller.hpp
    include/turtlebot3_maze_resolver/app/ros_qt_bridge.hpp

    include/turtlebot3_maze_resolver/infra/ros/maze_ros_bridge.hpp

    include/turtlebot3_maze_resolver/ui/pages/maze_solver_page.hpp
    include/turtlebot3_maze_resolver/ui/widgets/maze_widget.hpp

    include/turtlebot3_maze_resolver/ui/style/theme.hpp
    include/turtlebot3_maze_resolver/ui/style/theme_manager.hpp

    include/turtlebot3_maze_resolver/core/models/maze_types.hpp
    include/turtlebot3_maze_resolver/core/models/theme_id.hpp
    include/turtlebot3_maze_resolver/core/services/maze_session.hpp
)

# GUI sources:
set(GUI_SOURCES

    src/app/app_context.cpp
    src/app/main_window.cpp
    src/app/maze_execution_controller.cpp
    src/app/ros_qt_bridge.cpp

    src/infra/ros/maze_ros_bridge.cpp

    src/ui/pages/maze_solver_page.cpp
    src/ui/widgets/maze_widget.cpp

    src/ui/style/theme_manager.cpp
)

# Qt6 GUI executable configuration:
add_executable(
    turtlebot3_maze_gui

    src/gui_main.cpp

    ${GUI_HEADERS}
    ${GUI_SOURCES}
)

target_include_directories(
    turtlebot3_maze_gui

    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(
    turtlebot3_maze_gui

    ${PROJECT_NAME}_core

    Qt6::Core
    Qt6::Widgets
)

ament_target_dependencies(
    turtlebot3_maze_gui

    rclcpp
    geometry_msgs
    std_msgs
)

# Installation configuration:
install(
    TARGETS
        ${PROJECT_NAME}_core
        cell_motion_node
        turtlebot3_maze_gui
    
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(
    DIRECTORY include/
    DESTINATION include
)

install(
    DIRECTORY launch
    DESTINATION share/${PROJECT_NAME}
)

# Export configuration:
ament_export_include_directories(
    include
)

ament_export_libraries(
    ${PROJECT_NAME}_core
)

ament_package()