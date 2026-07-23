#include "turtlebot3_maze_resolver/infra/ros/maze_ros_bridge.hpp"

#include <chrono>
#include <utility>

namespace turtlebot3_maze_resolver::infra::ros
{

MazeRosBridge::MazeRosBridge(
    rclcpp::Node::SharedPtr node,
    QObject * parent)
    : QObject(parent),
      node_(std::move(node))
{
    target_publisher_ =
        node_->create_publisher<
        geometry_msgs::msg::PointStamped>(
            "maze/target",
            rclcpp::QoS(10));

    stop_publisher_ =
        node_->create_publisher<
        std_msgs::msg::Empty>(
            "maze/stop",
            rclcpp::QoS(10));

    motion_state_subscription_ =
        node_->create_subscription<
        std_msgs::msg::UInt8>(
            "maze/motion_state",
            rclcpp::QoS(10),

            [this](
                const std_msgs::msg::UInt8::
                SharedPtr message)
    {
        emit motionStateChanged(
            static_cast<int>(
                message->data));
    });

    connection_timer_ =
        node_->create_wall_timer(
            std::chrono::milliseconds(
                500),

    [this]() {
        checkConnection();
    });
}

void MazeRosBridge::sendTarget(
    const double x_m,
    const double y_m)
{
    geometry_msgs::msg::PointStamped message;

    message.header.stamp =
        node_->now();

    /*
     * Coordinates are expressed in the maze frame,
     * whose origin is the robot's initial pose.
     */
    message.header.frame_id =
        "start";

    message.point.x =
        x_m;

    message.point.y =
        y_m;

    message.point.z =
        0.0;

    target_publisher_->publish(
        message);
}

void MazeRosBridge::sendStop()
{
    std_msgs::msg::Empty message;

    stop_publisher_->publish(
        message);
}

void MazeRosBridge::checkConnection()
{
    const bool currently_connected =
        node_->count_publishers(
            "maze/motion_state") > 0;

    if (
        currently_connected ==
        connected_)
    {
        return;
    }

    connected_ =
        currently_connected;

    emit connectionChanged(
        connected_);
}

}  // namespace turtlebot3_maze_resolver::infra::ros