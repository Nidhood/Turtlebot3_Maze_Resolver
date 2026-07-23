#!/usr/bin/env python3

from launch import LaunchDescription

from launch.actions import (
    AppendEnvironmentVariable,
    DeclareLaunchArgument,
    OpaqueFunction,
)

from launch.substitutions import LaunchConfiguration

from launch_ros.actions import Node

from ament_index_python.packages import (
    get_package_share_directory,
)

import os


MAZE_COLUMNS = 16
MAZE_ROWS = 6

CELL_SIZE_M = 0.50

MAZE_WIDTH_M = (
    MAZE_COLUMNS *
    CELL_SIZE_M
)

MAZE_HEIGHT_M = (
    MAZE_ROWS *
    CELL_SIZE_M
)


def spawn_maze(context):

    start_row = int(
        LaunchConfiguration(
            "start_row"
        ).perform(context)
    )

    start_column = int(
        LaunchConfiguration(
            "start_column"
        ).perform(context)
    )

    robot_x = float(
        LaunchConfiguration(
            "robot_x"
        ).perform(context)
    )

    robot_y = float(
        LaunchConfiguration(
            "robot_y"
        ).perform(context)
    )

    entity_name = (
        LaunchConfiguration(
            "entity_name"
        ).perform(context)
    )

    if not (
        0 <= start_row <
        MAZE_ROWS
    ):
        raise RuntimeError(
            f"Invalid start_row={start_row}. "
            f"Expected 0..{MAZE_ROWS - 1}"
        )

    if not (
        0 <= start_column <
        MAZE_COLUMNS
    ):
        raise RuntimeError(
            f"Invalid start_column={start_column}. "
            f"Expected 0..{MAZE_COLUMNS - 1}"
        )

    cell_x_local = (
        -(MAZE_WIDTH_M / 2.0)
        +
        (
            start_column +
            0.5
        )
        *
        CELL_SIZE_M
    )

    cell_y_local = (
        +(MAZE_HEIGHT_M / 2.0)
        -
        (
            start_row +
            0.5
        )
        *
        CELL_SIZE_M
    )

    maze_x = (
        robot_x -
        cell_x_local
    )

    maze_y = (
        robot_y -
        cell_y_local
    )


    package_share = (
        get_package_share_directory(
            "turtlebot3_maze_gazebo"
        )
    )

    maze_sdf = os.path.join(
        package_share,
        "models",
        "school_maze",
        "sdf",
        "school_maze.sdf",
    )

    print("")
    print(
        "============================================"
    )

    print(
        " TurtleBot3 Maze Gazebo"
    )

    print(
        "============================================"
    )

    print(
        f"Start cell:"
        f" ({start_row}, {start_column})"
    )

    print(
        f"Robot world position:"
        f" ({robot_x:.3f}, {robot_y:.3f})"
    )

    print(
        f"Cell local position:"
        f" ({cell_x_local:.3f},"
        f" {cell_y_local:.3f})"
    )

    print(
        f"Maze spawn position:"
        f" ({maze_x:.3f},"
        f" {maze_y:.3f})"
    )

    print(
        "============================================"
    )

    print("")

    return [

        Node(

            package="ros_gz_sim",

            executable="create",

            name="spawn_school_maze",

            output="screen",

            arguments=[

                "-name",
                entity_name,

                "-file",
                maze_sdf,

                "-x",
                str(maze_x),

                "-y",
                str(maze_y),

                "-z",
                "0.0",

                "-R",
                "0.0",

                "-P",
                "0.0",

                "-Y",
                "0.0",
            ],
        )
    ]


def generate_launch_description():

    package_share = (
        get_package_share_directory(
            "turtlebot3_maze_gazebo"
        )
    )

    models_path = os.path.join(
        package_share,
        "models",
    )

    return LaunchDescription([


        AppendEnvironmentVariable(

            name="GZ_SIM_RESOURCE_PATH",

            value=models_path,
        ),

        DeclareLaunchArgument(

            "entity_name",

            default_value="school_maze",

            description=(
                "Gazebo entity name "
                "for the maze."
            ),
        ),

        DeclareLaunchArgument(

            "start_row",

            default_value="0",

            description=(
                "Maze row where "
                "TurtleBot3 starts."
            ),
        ),

        DeclareLaunchArgument(

            "start_column",

            default_value="0",

            description=(
                "Maze column where "
                "TurtleBot3 starts."
            ),
        ),

        DeclareLaunchArgument(

            "robot_x",

            default_value="0.0",

            description=(
                "Initial TurtleBot3 "
                "world X coordinate."
            ),
        ),

        DeclareLaunchArgument(

            "robot_y",

            default_value="0.0",

            description=(
                "Initial TurtleBot3 "
                "world Y coordinate."
            ),
        ),

        OpaqueFunction(
            function=spawn_maze
        ),
    ])