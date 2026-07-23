#pragma once

#include <QObject>

#include <geometry_msgs/msg/point_stamped.hpp>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/empty.hpp>
#include <std_msgs/msg/u_int8.hpp>

namespace turtlebot3_maze_resolver::infra::ros
{

class MazeRosBridge final : public QObject
{
    Q_OBJECT

public:
    enum MotionStatus : std::uint8_t
    {
        kIdle = 0,
        kAligning = 1,
        kDriving = 2,
        kSucceeded = 3,
        kAborted = 4
    };

    explicit MazeRosBridge(
        rclcpp::Node::SharedPtr node,
        QObject * parent = nullptr);

public slots:
    void sendTarget(
        double x_m,
        double y_m);

    void sendStop();

signals:
    void motionStateChanged(
        int status);

    void connectionChanged(
        bool connected);

private:
    void checkConnection();

private:
    rclcpp::Node::SharedPtr node_;

    rclcpp::Publisher<
    geometry_msgs::msg::PointStamped>::
    SharedPtr target_publisher_;

    rclcpp::Publisher<
    std_msgs::msg::Empty>::
    SharedPtr stop_publisher_;

    rclcpp::Subscription<
    std_msgs::msg::UInt8>::
    SharedPtr motion_state_subscription_;

    rclcpp::TimerBase::SharedPtr
    connection_timer_;

    bool connected_{
        false};
};

}  // namespace turtlebot3_maze_resolver::infra::ros