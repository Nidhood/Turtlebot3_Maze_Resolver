#include "turtlebot3_maze_resolver/app/main_window.hpp"

#include <QAction>
#include <QLabel>
#include <QMenu>
#include <QStatusBar>
#include <QStyle>
#include <QToolBar>
#include <QToolButton>

#include "turtlebot3_maze_resolver/core/models/theme_id.hpp"
#include "turtlebot3_maze_resolver/core/services/maze_session.hpp"
#include "turtlebot3_maze_resolver/ui/pages/maze_solver_page.hpp"
#include "turtlebot3_maze_resolver/ui/style/theme_manager.hpp"

namespace turtlebot3_maze_resolver::app
{

MainWindow::MainWindow(
    QWidget * parent)
    : QMainWindow(parent)
{
    setWindowTitle(
        "TurtleBot3 Maze Resolver");

    setMinimumSize(
        1200,
        760);

    resize(
        1400,
        850);

    buildUi();
    buildTopBar();
    buildStatusBar();
}

void MainWindow::setMazeSession(
    services::MazeSession * session)
{
    solver_page_->setSession(
        session);
}

void MainWindow::setConnected(
    const bool connected)
{
    connection_label_->setText(
        connected ?
        "ROS: Connected" :
        "ROS: Disconnected");
}

void MainWindow::setExecutionStatus(
    const QString & status)
{
    status_label_->setText(
        status);

    solver_page_->setExecutionStatus(
        status);
}

void MainWindow::setExecutionProgress(
    const int current,
    const int total)
{
    solver_page_->setExecutionProgress(
        current,
        total);
}

void MainWindow::buildUi()
{
    solver_page_ =
        new ui::pages::MazeSolverPage(
        this);

    setCentralWidget(
        solver_page_);

    connect(
        solver_page_,
        &ui::pages::MazeSolverPage::
        executeRequested,
        this,
        &MainWindow::executeRequested);

    connect(
        solver_page_,
        &ui::pages::MazeSolverPage::
        stopRequested,
        this,
        &MainWindow::stopRequested);
}

void MainWindow::buildTopBar()
{
    top_bar_ =
        new QToolBar(
        this);

    top_bar_->setObjectName(
        "topBar");

    top_bar_->setMovable(
        false);

    top_bar_->setFloatable(
        false);

    auto * title =
        new QLabel(
        "Maze Solver",
        this);

    title->setObjectName(
        "sectionTitle");

    top_bar_->addWidget(
        title);

    auto * spacer =
        new QWidget(
        this);

    spacer->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred);

    top_bar_->addWidget(
        spacer);

    theme_button_ =
        new QToolButton(
        this);

    theme_button_->setObjectName(
        "themeButton");

    theme_button_->setText(
        "Theme");

    theme_button_->setPopupMode(
        QToolButton::InstantPopup);

    theme_button_->setToolButtonStyle(
        Qt::ToolButtonTextBesideIcon);

    theme_button_->setIcon(
        style()->standardIcon(
            QStyle::SP_DesktopIcon));

    top_bar_->addWidget(
        theme_button_);

    addToolBar(
        Qt::TopToolBarArea,
        top_bar_);

    buildThemeMenu();
}

void MainWindow::buildStatusBar()
{
    connection_label_ =
        new QLabel(
        "ROS: Disconnected",
        this);

    status_label_ =
        new QLabel(
        "Ready",
        this);

    statusBar()->addWidget(
        status_label_,
        1);

    statusBar()->addPermanentWidget(
        connection_label_);
}

void MainWindow::buildThemeMenu()
{
    auto * menu =
        new QMenu(
        theme_button_);

    auto add_theme =
        [this, menu](
            const QString & name,
            const models::ThemeId id)
    {
        QAction * action =
            menu->addAction(
                name);

        connect(
            action,
            &QAction::triggered,
            this,
        [id]() {
            ui::style::ThemeManager::
            instance().apply(id);
        });
    };

    add_theme(
        "Light",
        models::ThemeId::kLight);

    add_theme(
        "Dark",
        models::ThemeId::kDark);

    add_theme(
        "Tron Ares",
        models::ThemeId::kTronAres);

    theme_button_->setMenu(
        menu);
}

}  // namespace turtlebot3_maze_resolver::app