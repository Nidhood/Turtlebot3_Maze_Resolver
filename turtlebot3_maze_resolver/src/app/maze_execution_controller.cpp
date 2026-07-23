#include "turtlebot3_maze_resolver/app/maze_execution_controller.hpp"

#include "turtlebot3_maze_resolver/core/services/maze_session.hpp"
#include "turtlebot3_maze_resolver/infra/ros/maze_ros_bridge.hpp"

namespace turtlebot3_maze_resolver::app
{

MazeExecutionController::
MazeExecutionController(
    services::MazeSession * session,
    infra::ros::MazeRosBridge * ros_bridge,
    QObject * parent)
    : QObject(parent),
      session_(session),
      ros_bridge_(ros_bridge)
{
}

void MazeExecutionController::
startExecution()
{
    if (
        session_ == nullptr ||
        ros_bridge_ == nullptr)
    {
        emit statusChanged(
            "Execution system unavailable.");

        return;
    }

    if (!session_->isSolved()) {
        emit statusChanged(
            "The maze route is not complete.");

        return;
    }

    targets_ =
        session_->metricTargets();

    /*
     * First target == start cell == (0, 0).
     *
     * The physical robot is already there.
     */
    if (!targets_.empty()) {
        targets_.erase(
            targets_.begin());
    }

    if (targets_.empty()) {
        emit statusChanged(
            "Nothing to execute.");

        return;
    }

    target_index_ =
        0;

    running_ =
        true;

    emit statusChanged(
        "Starting route execution...");

    sendCurrentTarget();
}

void MazeExecutionController::
stopExecution()
{
    if (ros_bridge_ != nullptr) {
        ros_bridge_->sendStop();
    }

    running_ =
        false;

    targets_.clear();

    target_index_ =
        0;

    emit statusChanged(
        "Execution stopped.");
}

void MazeExecutionController::
onMotionStateChanged(
    const int status)
{
    if (!running_) {
        return;
    }

    using Bridge =
        infra::ros::MazeRosBridge;

    switch (status) {
    case Bridge::kAligning:
        emit statusChanged(
            "Aligning with next cell...");
        break;

    case Bridge::kDriving:
        emit statusChanged(
            "Moving to next cell...");
        break;

    case Bridge::kSucceeded:
    {
        ++target_index_;

        if (
            target_index_ >=
            targets_.size())
        {
            running_ =
                false;

            emit progressChanged(
                static_cast<int>(
                    targets_.size()),

                static_cast<int>(
                    targets_.size()));

            emit statusChanged(
                "Route completed successfully.");

            emit executionFinished();

            return;
        }

        sendCurrentTarget();

        break;
    }

    case Bridge::kAborted:
        running_ =
            false;

        emit statusChanged(
            "Robot motion aborted.");

        break;

    case Bridge::kIdle:
    default:
        break;
    }
}

void MazeExecutionController::
sendCurrentTarget()
{
    if (
        !running_ ||
        target_index_ >=
        targets_.size())
    {
        return;
    }

    const auto & target =
        targets_[target_index_];

    emit progressChanged(
        static_cast<int>(
            target_index_ + 1),

        static_cast<int>(
            targets_.size()));

    emit statusChanged(
        QString(
            "Sending cell (%1, %2): "
            "x=%3 m, y=%4 m")
        .arg(
            target.source_cell.row)
        .arg(
            target.source_cell.column)
        .arg(
            target.x_m,
            0,
            'f',
            3)
        .arg(
            target.y_m,
            0,
            'f',
            3));

    ros_bridge_->sendTarget(
        target.x_m,
        target.y_m);
}

}  // namespace turtlebot3_maze_resolver::app