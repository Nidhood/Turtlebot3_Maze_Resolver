#include "turtlebot3_maze_resolver/app/ros_qt_bridge.hpp"

#include <QCoreApplication>
#include <QMetaObject>

#include <chrono>
#include <utility>

namespace turtlebot3_maze_resolver::app
{

RosQtBridge::RosQtBridge(
    QApplication & application,
    rclcpp::Node::SharedPtr node)
    : application_(application),
      node_(std::move(node))
{
    executor_.add_node(
        node_);

    running_.store(
        true);

    QObject::connect(
        &application_,
        &QCoreApplication::aboutToQuit,
    [&]() {
        stop();
    });

    ros_thread_ =
        std::thread(
    [this]() {
        spinLoop();
    });

    shutdown_thread_ =
        std::thread(
    [this]() {
        shutdownWatchLoop();
    });
}

RosQtBridge::~RosQtBridge()
{
    stopAndJoin();
}

void RosQtBridge::stop() noexcept
{
    running_.store(
        false);

    executor_.cancel();

    if (rclcpp::ok()) {
        rclcpp::shutdown();
    }
}

void RosQtBridge::spinLoop()
{
    using namespace
        std::chrono_literals;

    while (
        running_.load() &&
        rclcpp::ok())
    {
        executor_.spin_some();

        std::this_thread::sleep_for(
            2ms);
    }
}

void RosQtBridge::shutdownWatchLoop()
{
    using namespace
        std::chrono_literals;

    while (
        running_.load() &&
        rclcpp::ok())
    {
        std::this_thread::sleep_for(
            50ms);
    }

    QMetaObject::invokeMethod(
        &application_,
        "quit",
        Qt::QueuedConnection);
}

void RosQtBridge::stopAndJoin() noexcept
{
    stop();

    if (ros_thread_.joinable()) {
        ros_thread_.join();
    }

    if (shutdown_thread_.joinable()) {
        shutdown_thread_.join();
    }
}

}  // namespace turtlebot3_maze_resolver::app