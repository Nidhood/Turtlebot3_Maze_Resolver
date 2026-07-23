#pragma once

#include <QWidget>

#include "turtlebot3_maze_resolver/core/models/maze_types.hpp"

class QLabel;
class QPushButton;
class QDoubleSpinBox;

namespace turtlebot3_maze_resolver::services
{
class MazeSession;
}

namespace turtlebot3_maze_resolver::ui::widgets
{
class MazeWidget;
}

namespace turtlebot3_maze_resolver::ui::pages
{

class MazeSolverPage final : public QWidget
{
    Q_OBJECT

public:
    explicit MazeSolverPage(
        QWidget * parent = nullptr);

    void setSession(
        services::MazeSession * session);

    void setExecutionStatus(
        const QString & status);

    void setExecutionProgress(
        int current,
        int total);

signals:
    void executeRequested();

    void stopRequested();

private:
    void buildUi();

    void connectSignals();

    void loadMaze();

    void setInteractionMode(
        models::MazeInteractionMode mode);

    void handleCellClicked(
        int row,
        int column);

    void refresh();

private:
    services::MazeSession * session_{
        nullptr};

    models::MazeInteractionMode interaction_mode_{
        models::MazeInteractionMode::kNone};

    widgets::MazeWidget * maze_widget_{
        nullptr};

    QLabel * maze_size_label_{
        nullptr};

    QLabel * start_label_{
        nullptr};

    QLabel * goal_label_{
        nullptr};

    QLabel * path_label_{
        nullptr};

    QLabel * mode_label_{
        nullptr};

    QLabel * execution_label_{
        nullptr};

    QLabel * progress_label_{
        nullptr};

    QDoubleSpinBox * cell_size_spin_{
        nullptr};

    QPushButton * load_button_{
        nullptr};

    QPushButton * start_button_{
        nullptr};

    QPushButton * goal_button_{
        nullptr};

    QPushButton * path_button_{
        nullptr};

    QPushButton * undo_button_{
        nullptr};

    QPushButton * clear_button_{
        nullptr};

    QPushButton * execute_button_{
        nullptr};

    QPushButton * stop_button_{
        nullptr};
};

}  // namespace turtlebot3_maze_resolver::ui::pages