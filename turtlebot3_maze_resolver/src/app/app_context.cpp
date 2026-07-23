#include "turtlebot3_maze_resolver/app/app_context.hpp"

#include <QObject>

#include "turtlebot3_maze_resolver/app/maze_execution_controller.hpp"
#include "turtlebot3_maze_resolver/infra/ros/maze_ros_bridge.hpp"

namespace turtlebot3_maze_resolver::app
{

AppContext::AppContext()
    : node_(
          std::make_shared<rclcpp::Node>(
              "turtlebot3_maze_gui_node")),
      maze_session_(),
      main_window_(nullptr)
{
    ros_bridge_ =
        std::make_unique<
        infra::ros::MazeRosBridge>(
            node_);

    execution_controller_ =
        std::make_unique<
        MazeExecutionController>(
            &maze_session_,
            ros_bridge_.get());

    main_window_.setMazeSession(
        &maze_session_);

    /*
     * UI -> application execution workflow.
     */

    QObject::connect(
        &main_window_,
        &MainWindow::executeRequested,
        execution_controller_.get(),
        &MazeExecutionController::
        startExecution);

    QObject::connect(
        &main_window_,
        &MainWindow::stopRequested,
        execution_controller_.get(),
        &MazeExecutionController::
        stopExecution);

    /*
     * ROS -> application execution workflow.
     */

    QObject::connect(
        ros_bridge_.get(),
        &infra::ros::MazeRosBridge::
        motionStateChanged,

        execution_controller_.get(),
        &MazeExecutionController::
        onMotionStateChanged,

        Qt::QueuedConnection);

    /*
     * Execution workflow -> UI.
     */

    QObject::connect(
        execution_controller_.get(),
        &MazeExecutionController::
        statusChanged,

        &main_window_,
        &MainWindow::
        setExecutionStatus,

        Qt::QueuedConnection);

    QObject::connect(
        execution_controller_.get(),
        &MazeExecutionController::
        progressChanged,

        &main_window_,
        &MainWindow::
        setExecutionProgress,

        Qt::QueuedConnection);

    /*
     * ROS connectivity -> UI.
     */

    QObject::connect(
        ros_bridge_.get(),
        &infra::ros::MazeRosBridge::
        connectionChanged,

        &main_window_,
        &MainWindow::
        setConnected,

        Qt::QueuedConnection);
}

AppContext::~AppContext()
{
    execution_controller_.reset();

    ros_bridge_.reset();

    node_.reset();
}

MainWindow &
AppContext::mainWindow() noexcept
{
    return main_window_;
}

rclcpp::Node::SharedPtr
AppContext::node() const noexcept
{
    return node_;
}

}  // namespace turtlebot3_maze_resolver::app