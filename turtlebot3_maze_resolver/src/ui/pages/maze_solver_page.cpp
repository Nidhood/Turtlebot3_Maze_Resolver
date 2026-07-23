#include "turtlebot3_maze_resolver/ui/pages/maze_solver_page.hpp"

#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <string>

#include "turtlebot3_maze_resolver/core/services/maze_session.hpp"
#include "turtlebot3_maze_resolver/ui/widgets/maze_widget.hpp"

namespace turtlebot3_maze_resolver::ui::pages
{

namespace
{

QFrame * createPanel(
    QWidget * parent)
{
    auto * panel =
        new QFrame(parent);

    panel->setObjectName(
        "panel");

    return panel;
}

QLabel * createSectionTitle(
    const QString & text,
    QWidget * parent)
{
    auto * label =
        new QLabel(
        text,
        parent);

    label->setObjectName(
        "sectionTitle");

    return label;
}

QString coordinateText(
    const std::optional<models::CellCoord> & coordinate)
{
    if (!coordinate.has_value()) {
        return "--";
    }

    return QString(
               "(%1, %2)")
           .arg(
               coordinate->row)
           .arg(
               coordinate->column);
}

}  // namespace

MazeSolverPage::MazeSolverPage(
    QWidget * parent)
    : QWidget(parent)
{
    buildUi();
    connectSignals();

    refresh();
}

void MazeSolverPage::setSession(
    services::MazeSession * session)
{
    session_ =
        session;

    maze_widget_->setSession(
        session_);

    if (session_ != nullptr) {
        cell_size_spin_->setValue(
            session_->cellSizeMeters());
    }

    refresh();
}

void MazeSolverPage::setExecutionStatus(
    const QString & status)
{
    execution_label_->setText(
        status);
}

void MazeSolverPage::setExecutionProgress(
    const int current,
    const int total)
{
    progress_label_->setText(
        QString("%1 / %2")
        .arg(current)
        .arg(total));
}

void MazeSolverPage::buildUi()
{
    auto * root =
        new QVBoxLayout(this);

    root->setContentsMargins(
        24,
        20,
        24,
        20);

    root->setSpacing(
        16);

    auto * title =
        new QLabel(
        "TurtleBot3 Maze Resolver",
        this);

    title->setObjectName(
        "pageTitle");

    auto * subtitle =
        new QLabel(
        "Load a maze, define start and goal, solve it manually cell by cell, "
        "then execute the same route on TurtleBot3.",
        this);

    subtitle->setObjectName(
        "pageSubtitle");

    root->addWidget(
        title);

    root->addWidget(
        subtitle);

    auto * content =
        new QHBoxLayout;

    content->setSpacing(
        16);

    /*
     * ============================================================
     * Left: maze visualization
     * ============================================================
     */

    auto * maze_panel =
        createPanel(this);

    auto * maze_layout =
        new QVBoxLayout(
        maze_panel);

    maze_layout->setContentsMargins(
        14,
        14,
        14,
        14);

    maze_widget_ =
        new widgets::MazeWidget(
        maze_panel);

    maze_layout->addWidget(
        maze_widget_,
        1);

    content->addWidget(
        maze_panel,
        3);

    /*
     * ============================================================
     * Right: controls
     * ============================================================
     */

    auto * controls_column =
        new QVBoxLayout;

    controls_column->setSpacing(
        14);

    /*
     * Maze configuration panel.
     */

    auto * config_panel =
        createPanel(this);

    auto * config_layout =
        new QVBoxLayout(
        config_panel);

    config_layout->setContentsMargins(
        16,
        16,
        16,
        16);

    config_layout->setSpacing(
        10);

    config_layout->addWidget(
        createSectionTitle(
            "Maze configuration",
            config_panel));

    load_button_ =
        new QPushButton(
        "Load .in Maze",
        config_panel);

    config_layout->addWidget(
        load_button_);

    auto * size_grid =
        new QGridLayout;

    size_grid->addWidget(
        new QLabel(
            "Maze size:",
            config_panel),
        0,
        0);

    maze_size_label_ =
        new QLabel(
        "--",
        config_panel);

    size_grid->addWidget(
        maze_size_label_,
        0,
        1);

    size_grid->addWidget(
        new QLabel(
            "Cell size:",
            config_panel),
        1,
        0);

    cell_size_spin_ =
        new QDoubleSpinBox(
        config_panel);

    cell_size_spin_->setRange(
        0.01,
        10.0);

    cell_size_spin_->setDecimals(
        3);

    cell_size_spin_->setSingleStep(
        0.05);

    cell_size_spin_->setValue(
        0.50);

    cell_size_spin_->setSuffix(
        " m");

    size_grid->addWidget(
        cell_size_spin_,
        1,
        1);

    size_grid->addWidget(
        new QLabel(
            "Start:",
            config_panel),
        2,
        0);

    start_label_ =
        new QLabel(
        "--",
        config_panel);

    size_grid->addWidget(
        start_label_,
        2,
        1);

    size_grid->addWidget(
        new QLabel(
            "Goal:",
            config_panel),
        3,
        0);

    goal_label_ =
        new QLabel(
        "--",
        config_panel);

    size_grid->addWidget(
        goal_label_,
        3,
        1);

    config_layout->addLayout(
        size_grid);

    auto * selection_row =
        new QHBoxLayout;

    start_button_ =
        new QPushButton(
        "Set Start",
        config_panel);

    goal_button_ =
        new QPushButton(
        "Set Goal",
        config_panel);

    selection_row->addWidget(
        start_button_);

    selection_row->addWidget(
        goal_button_);

    config_layout->addLayout(
        selection_row);

    controls_column->addWidget(
        config_panel);

    /*
     * Path panel.
     */

    auto * path_panel =
        createPanel(this);

    auto * path_layout =
        new QVBoxLayout(
        path_panel);

    path_layout->setContentsMargins(
        16,
        16,
        16,
        16);

    path_layout->setSpacing(
        10);

    path_layout->addWidget(
        createSectionTitle(
            "Manual solution",
            path_panel));

    mode_label_ =
        new QLabel(
        "Mode: idle",
        path_panel);

    mode_label_->setObjectName(
        "muted");

    path_layout->addWidget(
        mode_label_);

    path_button_ =
        new QPushButton(
        "Build Path",
        path_panel);

    path_layout->addWidget(
        path_button_);

    auto * edit_row =
        new QHBoxLayout;

    undo_button_ =
        new QPushButton(
        "Undo",
        path_panel);

    undo_button_->setObjectName(
        "secondaryButton");

    clear_button_ =
        new QPushButton(
        "Clear",
        path_panel);

    clear_button_->setObjectName(
        "secondaryButton");

    edit_row->addWidget(
        undo_button_);

    edit_row->addWidget(
        clear_button_);

    path_layout->addLayout(
        edit_row);

    path_label_ =
        new QLabel(
        "Path cells: 0",
        path_panel);

    path_label_->setObjectName(
        "muted");

    path_layout->addWidget(
        path_label_);

    controls_column->addWidget(
        path_panel);

    /*
     * Execution panel.
     */

    auto * execution_panel =
        createPanel(this);

    auto * execution_layout =
        new QVBoxLayout(
        execution_panel);

    execution_layout->setContentsMargins(
        16,
        16,
        16,
        16);

    execution_layout->setSpacing(
        10);

    execution_layout->addWidget(
        createSectionTitle(
            "Robot execution",
            execution_panel));

    execution_label_ =
        new QLabel(
        "Ready",
        execution_panel);

    progress_label_ =
        new QLabel(
        "0 / 0",
        execution_panel);

    progress_label_->setObjectName(
        "muted");

    execution_layout->addWidget(
        execution_label_);

    execution_layout->addWidget(
        progress_label_);

    execute_button_ =
        new QPushButton(
        "Execute Route",
        execution_panel);

    stop_button_ =
        new QPushButton(
        "Emergency Stop",
        execution_panel);

    stop_button_->setObjectName(
        "dangerButton");

    execution_layout->addWidget(
        execute_button_);

    execution_layout->addWidget(
        stop_button_);

    controls_column->addWidget(
        execution_panel);

    controls_column->addStretch(
        1);

    content->addLayout(
        controls_column,
        1);

    root->addLayout(
        content,
        1);
}

void MazeSolverPage::connectSignals()
{
    connect(
        load_button_,
        &QPushButton::clicked,
        this,
        &MazeSolverPage::loadMaze);

    connect(
        start_button_,
        &QPushButton::clicked,
        this,
    [this]() {
        setInteractionMode(
            models::MazeInteractionMode::
            kSelectStart);
    });

    connect(
        goal_button_,
        &QPushButton::clicked,
        this,
    [this]() {
        setInteractionMode(
            models::MazeInteractionMode::
            kSelectGoal);
    });

    connect(
        path_button_,
        &QPushButton::clicked,
        this,
    [this]() {
        setInteractionMode(
            models::MazeInteractionMode::
            kBuildPath);
    });

    connect(
        undo_button_,
        &QPushButton::clicked,
        this,
    [this]() {
        if (session_ == nullptr) {
            return;
        }

        session_->undoPathStep();

        refresh();
    });

    connect(
        clear_button_,
        &QPushButton::clicked,
        this,
    [this]() {
        if (session_ == nullptr) {
            return;
        }

        session_->clearPath();

        refresh();
    });

    connect(
        cell_size_spin_,
        &QDoubleSpinBox::valueChanged,
        this,
    [this](double value) {
        if (session_ == nullptr) {
            return;
        }

        session_->setCellSizeMeters(
            value);
    });

    connect(
        maze_widget_,
        &widgets::MazeWidget::cellClicked,
        this,
        &MazeSolverPage::handleCellClicked);

    connect(
        execute_button_,
        &QPushButton::clicked,
        this,
    [this]() {
        if (
            session_ == nullptr ||
            !session_->isSolved())
        {
            QMessageBox::warning(
                this,
                "Route not complete",
                "The manual path must reach the goal "
                "before it can be executed.");

            return;
        }

        emit executeRequested();
    });

    connect(
        stop_button_,
        &QPushButton::clicked,
        this,
        &MazeSolverPage::stopRequested);
}

void MazeSolverPage::loadMaze()
{
    if (session_ == nullptr) {
        return;
    }

    const QString file_path =
        QFileDialog::getOpenFileName(
            this,
            "Open maze",
            QString(),
            "Maze files (*.in *.txt);;"
            "All files (*)");

    if (file_path.isEmpty()) {
        return;
    }

    std::string error;

    if (
        !session_->loadFromFile(
            file_path.toStdString(),
            &error))
    {
        QMessageBox::critical(
            this,
            "Maze loading error",
            QString::fromStdString(
                error));

        return;
    }

    setInteractionMode(
        models::MazeInteractionMode::
        kSelectStart);

    execution_label_->setText(
        "Maze loaded. Select the start cell.");

    refresh();
}

void MazeSolverPage::setInteractionMode(
    const models::MazeInteractionMode mode)
{
    interaction_mode_ =
        mode;

    switch (mode) {
    case models::MazeInteractionMode::kSelectStart:
        mode_label_->setText(
            "Mode: click the START cell");
        break;

    case models::MazeInteractionMode::kSelectGoal:
        mode_label_->setText(
            "Mode: click the GOAL cell");
        break;

    case models::MazeInteractionMode::kBuildPath:
        mode_label_->setText(
            "Mode: solve cell by cell");
        break;

    case models::MazeInteractionMode::kNone:
    default:
        mode_label_->setText(
            "Mode: idle");
        break;
    }
}

void MazeSolverPage::handleCellClicked(
    const int row,
    const int column)
{
    if (
        session_ == nullptr ||
        !session_->loaded())
    {
        return;
    }

    const models::CellCoord coordinate{
        row,
        column};

    switch (interaction_mode_) {
    case models::MazeInteractionMode::
            kSelectStart:
        {
            if (
                session_->setStart(
                    coordinate))
            {
                setInteractionMode(
                    models::MazeInteractionMode::
                    kSelectGoal);
            }

            break;
        }

    case models::MazeInteractionMode::
            kSelectGoal:
        {
            if (
                session_->setGoal(
                    coordinate))
            {
                setInteractionMode(
                    models::MazeInteractionMode::
                    kBuildPath);
            }

            break;
        }

    case models::MazeInteractionMode::
            kBuildPath:
        {
            std::string error;

            if (
                !session_->appendPathCell(
                    coordinate,
                    &error))
            {
                execution_label_->setText(
                    QString::fromStdString(
                        error));
            } else if (
                session_->isSolved())
            {
                execution_label_->setText(
                    "Maze solved. Route ready for execution.");
            }

            break;
        }

    case models::MazeInteractionMode::kNone:
    default:
        break;
    }

    refresh();
}

void MazeSolverPage::refresh()
{
    const bool loaded =
        session_ != nullptr &&
        session_->loaded();

    start_button_->setEnabled(
        loaded);

    goal_button_->setEnabled(
        loaded);

    path_button_->setEnabled(
        loaded);

    undo_button_->setEnabled(
        loaded);

    clear_button_->setEnabled(
        loaded);

    execute_button_->setEnabled(
        loaded &&
        session_->isSolved());

    if (!loaded) {
        maze_size_label_->setText(
            "--");

        start_label_->setText(
            "--");

        goal_label_->setText(
            "--");

        path_label_->setText(
            "Path cells: 0");

        maze_widget_->update();

        return;
    }

    maze_size_label_->setText(
        QString("%1 × %2")
        .arg(session_->columns())
        .arg(session_->rows()));

    start_label_->setText(
        coordinateText(
            session_->start()));

    goal_label_->setText(
        coordinateText(
            session_->goal()));

    path_label_->setText(
        QString("Path cells: %1")
        .arg(
            session_->path().size()));

    maze_widget_->update();
}

}  // namespace turtlebot3_maze_resolver::ui::pages