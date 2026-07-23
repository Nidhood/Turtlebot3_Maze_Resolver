from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration

from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description():

    # ============================================================
    # Launch arguments
    # ============================================================

    use_sim_time = LaunchConfiguration("use_sim_time")

    use_stamped_cmd_vel = LaunchConfiguration(
        "use_stamped_cmd_vel"
    )

    odom_topic = LaunchConfiguration(
        "odom_topic"
    )

    cmd_vel_topic = LaunchConfiguration(
        "cmd_vel_topic"
    )

    # ============================================================
    # Cell motion controller
    # ============================================================

    cell_motion_node = Node(
        package="turtlebot3_maze_resolver",
        executable="cell_motion_node",
        name="cell_motion_node",

        output="screen",

        parameters=[
            {
                # ------------------------------------------------
                # ROS environment
                # ------------------------------------------------

                "use_sim_time":
                    ParameterValue(
                        use_sim_time,
                        value_type=bool,
                    ),

                "use_stamped_cmd_vel":
                    ParameterValue(
                        use_stamped_cmd_vel,
                        value_type=bool,
                    ),

                "odom_topic":
                    odom_topic,

                "cmd_vel_topic":
                    cmd_vel_topic,

                # ------------------------------------------------
                # Controller
                # ------------------------------------------------

                "control_frequency_hz":
                    50.0,

                "kp_linear":
                    1.20,

                "kp_angular":
                    2.50,

                "kp_drive_angular":
                    2.00,

                # ------------------------------------------------
                # Velocity limits
                # ------------------------------------------------

                "max_linear_velocity":
                    0.12,

                "max_angular_velocity":
                    1.00,

                # ------------------------------------------------
                # Precision
                # ------------------------------------------------

                "position_tolerance":
                    0.005,

                "heading_tolerance":
                    0.03,

                "realign_threshold":
                    0.15,

                # ------------------------------------------------
                # Safety
                # ------------------------------------------------

                "motion_timeout_sec":
                    30.0,

                "odom_timeout_sec":
                    1.0,
            }
        ],
    )

    # ============================================================
    # Qt6 GUI
    # ============================================================

    maze_gui = Node(
        package="turtlebot3_maze_resolver",
        executable="turtlebot3_maze_gui",
        name="turtlebot3_maze_gui_node",

        output="screen",

        parameters=[
            {
                "use_sim_time":
                    ParameterValue(
                        use_sim_time,
                        value_type=bool,
                    ),
            }
        ],
    )

    # ============================================================
    # Launch description
    # ============================================================

    return LaunchDescription([

        # --------------------------------------------------------
        # Simulation / real robot clock
        # --------------------------------------------------------

        DeclareLaunchArgument(
            "use_sim_time",

            default_value="false",

            description=(
                "Use simulation clock. "
                "Set true when running Gazebo, "
                "false for the real TurtleBot3."
            ),
        ),

        # --------------------------------------------------------
        # cmd_vel message type
        # --------------------------------------------------------

        DeclareLaunchArgument(
            "use_stamped_cmd_vel",

            default_value="true",

            description=(
                "Publish geometry_msgs/TwistStamped "
                "instead of geometry_msgs/Twist."
            ),
        ),

        # --------------------------------------------------------
        # Topic configuration
        # --------------------------------------------------------

        DeclareLaunchArgument(
            "odom_topic",

            default_value="odom",

            description=(
                "Odometry topic used by "
                "the cell motion controller."
            ),
        ),

        DeclareLaunchArgument(
            "cmd_vel_topic",

            default_value="cmd_vel",

            description=(
                "Velocity command topic used "
                "to control TurtleBot3."
            ),
        ),

        # --------------------------------------------------------
        # Application
        # --------------------------------------------------------

        cell_motion_node,
        maze_gui,
    ])