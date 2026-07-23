#pragma once

#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include "geometry_msgs/msg/point_stamped.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "std_msgs/msg/empty.hpp"
#include "std_msgs/msg/u_int8.hpp"

#include "turtlebot3_maze_resolver/cell_motion_controller.hpp"
#include "turtlebot3_maze_resolver/motion_types.hpp"

namespace turtlebot3_maze_resolver
{

class CellMotionNode final : public rclcpp::Node
{
public:
    CellMotionNode();

    ~CellMotionNode() override;

    rclcpp::Subscription<
    geometry_msgs::msg::PointStamped>::
    SharedPtr maze_target_subscription_;

    rclcpp::Subscription<
    std_msgs::msg::Empty>::
    SharedPtr maze_stop_subscription_;

    rclcpp::Publisher<
    std_msgs::msg::UInt8>::
    SharedPtr motion_state_publisher_;

    Pose2D start_pose_{};

    bool start_pose_initialized_{
        false};

private:
    void odometryCallback(
        const nav_msgs::msg::Odometry::SharedPtr message);

    void controlLoop();

    void initializeTarget(const Pose2D & initial_pose);

    void publishCommand(
        const VelocityCommand & command);

    void publishStop();

    [[nodiscard]]
    static double quaternionToYaw(
        double x,
        double y,
        double z,
        double w) noexcept;

    // ROS interfaces.
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr
    odometry_subscription_;

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr
    twist_publisher_;

    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr
    twist_stamped_publisher_;

    rclcpp::TimerBase::SharedPtr control_timer_;

    // Pure motion controller.
    std::unique_ptr<CellMotionController> controller_;

    // Requested target.
    double requested_target_x_m_{0.0};
    double requested_target_y_m_{0.0};

    std::string target_frame_;
    std::string odom_topic_;
    std::string cmd_vel_topic_;
    std::string command_frame_id_;

    bool use_stamped_cmd_vel_{true};

    // Timing and safety.
    double control_frequency_hz_{50.0};
    double motion_timeout_sec_{30.0};
    double odom_timeout_sec_{1.0};

    std::chrono::steady_clock::time_point last_odom_time_;
    std::chrono::steady_clock::time_point motion_start_time_;

    // Runtime state.
    Pose2D latest_pose_{};

    bool has_odom_{false};
    bool target_initialized_{false};
    bool motion_finished_{false};

    MotionState previous_state_{MotionState::kIdle};

    void mazeTargetCallback(
        const geometry_msgs::msg::PointStamped::
        SharedPtr message);

    void mazeStopCallback(
        const std_msgs::msg::Empty::
        SharedPtr message);

    void publishMotionState(
        std::uint8_t state);

    [[nodiscard]]
    Target2D resolveStartFrameTarget(
        double x_m,
        double y_m) const;
};

}  // namespace turtlebot3_maze_resolver