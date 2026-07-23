#pragma once

#include <QApplication>

#include <rclcpp/executors/single_threaded_executor.hpp>
#include <rclcpp/rclcpp.hpp>

#include <atomic>
#include <thread>

namespace turtlebot3_maze_resolver::app
{

class RosQtBridge final
{
public:
    RosQtBridge(
        QApplication & application,
        rclcpp::Node::SharedPtr node);

    ~RosQtBridge();

    RosQtBridge(
        const RosQtBridge &) = delete;

    RosQtBridge &
    operator=(
        const RosQtBridge &) = delete;

    void stop() noexcept;

private:
    void spinLoop();

    void shutdownWatchLoop();

    void stopAndJoin() noexcept;

private:
    QApplication & application_;

    rclcpp::Node::SharedPtr node_;

    rclcpp::executors::
    SingleThreadedExecutor executor_;

    std::atomic<bool> running_{
        false};

    std::thread ros_thread_;

    std::thread shutdown_thread_;
};

}  // namespace turtlebot3_maze_resolver::app