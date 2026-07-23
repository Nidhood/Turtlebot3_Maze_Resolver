#include <exception>
#include <memory>
#include <rclcpp/rclcpp.hpp>

#include "turtlebot3_maze_resolver/cell_motion_node.hpp"
#include "turtlebot3_maze_resolver/constants.hpp"

int main( int argc, char  * argv[] ) {

    // 2. ROS after Qt:
    rclcpp::init(argc, argv);

    int exit_code = 0;

    try {
        const auto node = std::make_shared<turtlebot3_maze_resolver::CellMotionNode>();
        rclcpp::spin(node);
    }
    catch (const std::exception & exception) {
        RCLCPP_FATAL( rclcpp::get_logger( turtlebot3_maze_resolver::constants::kNodeName), "Fatal error: %s", exception.what() );
        exit_code = 1;
    }

    if (rclcpp::ok()) {
        rclcpp::shutdown();
    }

    return exit_code;
}