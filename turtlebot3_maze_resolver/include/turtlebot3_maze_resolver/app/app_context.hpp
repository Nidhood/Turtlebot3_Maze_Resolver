#pragma once

#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "turtlebot3_maze_resolver/app/main_window.hpp"
#include "turtlebot3_maze_resolver/core/services/maze_session.hpp"

namespace turtlebot3_maze_resolver::infra::ros
{
class MazeRosBridge;
}

namespace turtlebot3_maze_resolver::app
{

class MazeExecutionController;

class AppContext final
{
public:
    AppContext();

    ~AppContext();

    [[nodiscard]]
    MainWindow &
    mainWindow() noexcept;

    [[nodiscard]]
    rclcpp::Node::SharedPtr
    node() const noexcept;

private:
    rclcpp::Node::SharedPtr
    node_;

    services::MazeSession
    maze_session_;

    MainWindow
    main_window_;

    std::unique_ptr<
    infra::ros::MazeRosBridge>
    ros_bridge_;

    std::unique_ptr<
    MazeExecutionController>
    execution_controller_;
};

}  // namespace turtlebot3_maze_resolver::app