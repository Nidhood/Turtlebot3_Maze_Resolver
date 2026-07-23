#pragma once

#include <QObject>

#include <cstddef>
#include <vector>

#include "turtlebot3_maze_resolver/core/models/maze_types.hpp"

namespace turtlebot3_maze_resolver::services
{
class MazeSession;
}

namespace turtlebot3_maze_resolver::infra::ros
{
class MazeRosBridge;
}

namespace turtlebot3_maze_resolver::app
{

class MazeExecutionController final : public QObject
{
    Q_OBJECT

public:
    MazeExecutionController(
        services::MazeSession * session,
        infra::ros::MazeRosBridge * ros_bridge,
        QObject * parent = nullptr);

public slots:
    void startExecution();

    void stopExecution();

    void onMotionStateChanged(
        int status);

signals:
    void statusChanged(
        const QString & status);

    void progressChanged(
        int current,
        int total);

    void executionFinished();

private:
    void sendCurrentTarget();

private:
    services::MazeSession * session_{
        nullptr};

    infra::ros::MazeRosBridge * ros_bridge_{
        nullptr};

    std::vector<
    models::MetricTarget> targets_;

    std::size_t target_index_{
        0};

    bool running_{
        false};
};

}  // namespace turtlebot3_maze_resolver::app