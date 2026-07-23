#pragma once

#include <QMainWindow>

class QLabel;
class QToolBar;
class QToolButton;

namespace turtlebot3_maze_resolver::services
{
class MazeSession;
}

namespace turtlebot3_maze_resolver::ui::pages
{
class MazeSolverPage;
}

namespace turtlebot3_maze_resolver::app
{

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(
        QWidget * parent = nullptr);

    void setMazeSession(
        services::MazeSession * session);

public slots:
    void setConnected(
        bool connected);

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

    void buildTopBar();

    void buildStatusBar();

    void buildThemeMenu();

private:
    ui::pages::MazeSolverPage *
    solver_page_{nullptr};

    QToolBar *
    top_bar_{nullptr};

    QToolButton *
    theme_button_{nullptr};

    QLabel *
    connection_label_{nullptr};

    QLabel *
    status_label_{nullptr};
};

}  // namespace turtlebot3_maze_resolver::app