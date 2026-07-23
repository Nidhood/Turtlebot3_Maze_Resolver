#include "turtlebot3_maze_resolver/core/services/maze_session.hpp"

#include <cmath>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace turtlebot3_maze_resolver::services
{

namespace
{

bool horizontalWallExists(
    const std::string & line,
    const int column)
{
    const std::size_t start =
        static_cast<std::size_t>(
            (column * 4) + 1);

    if (start + 2 >= line.size()) {
        return true;
    }

    return
        line[start] == '-' ||
        line[start + 1] == '-' ||
        line[start + 2] == '-';
}

bool verticalWallExists(
    const std::string & line,
    const int character_index)
{
    if (
        character_index < 0 ||
        static_cast<std::size_t>(
            character_index) >= line.size())
    {
        return true;
    }

    return
        line[
            static_cast<std::size_t>(
                character_index)] == '|';
}

}  // namespace

bool MazeSession::loadFromFile(
    const std::string & file_path,
    std::string * error_message)
{
    std::ifstream input(file_path);

    if (!input.is_open()) {
        if (error_message != nullptr) {
            *error_message =
                "Could not open maze file.";
        }

        return false;
    }

    std::vector<std::string> lines;

    std::string line;

    while (std::getline(input, line)) {
        if (
            !line.empty() &&
            line.back() == '\r')
        {
            line.pop_back();
        }

        lines.push_back(line);
    }

    while (
        !lines.empty() &&
        lines.back().empty())
    {
        lines.pop_back();
    }

    if (lines.size() < 3) {
        if (error_message != nullptr) {
            *error_message =
                "Maze must contain at least one row.";
        }

        return false;
    }

    if ((lines.size() % 2) == 0) {
        if (error_message != nullptr) {
            *error_message =
                "Invalid maze height. Expected 2*N+1 text lines.";
        }

        return false;
    }

    const std::size_t first_width =
        lines.front().size();

    if (
        first_width < 5 ||
        ((first_width - 1) % 4) != 0)
    {
        if (error_message != nullptr) {
            *error_message =
                "Invalid maze width. Expected 4*N+1 characters.";
        }

        return false;
    }

    const int parsed_rows =
        static_cast<int>(
            (lines.size() - 1) / 2);

    const int parsed_columns =
        static_cast<int>(
            (first_width - 1) / 4);

    const std::size_t required_width =
        static_cast<std::size_t>(
            (parsed_columns * 4) + 1);

    for (const auto & maze_line : lines) {
        if (maze_line.size() < required_width) {
            if (error_message != nullptr) {
                *error_message =
                    "Maze contains lines with inconsistent width.";
            }

            return false;
        }
    }

    std::vector<MazeCell> parsed_cells;

    parsed_cells.resize(
        static_cast<std::size_t>(
            parsed_rows * parsed_columns));

    for (
        int row = 0;
        row < parsed_rows;
        ++row)
    {
        for (
            int column = 0;
            column < parsed_columns;
            ++column)
        {
            MazeCell parsed_cell{};

            const std::string & top_line =
                lines[
                    static_cast<std::size_t>(
                        row * 2)];

            const std::string & middle_line =
                lines[
                    static_cast<std::size_t>(
                        (row * 2) + 1)];

            const std::string & bottom_line =
                lines[
                    static_cast<std::size_t>(
                        (row * 2) + 2)];

            parsed_cell.top_wall =
                horizontalWallExists(
                    top_line,
                    column);

            parsed_cell.bottom_wall =
                horizontalWallExists(
                    bottom_line,
                    column);

            parsed_cell.left_wall =
                verticalWallExists(
                    middle_line,
                    column * 4);

            parsed_cell.right_wall =
                verticalWallExists(
                    middle_line,
                    (column * 4) + 4);

            const std::size_t index =
                static_cast<std::size_t>(
                    (row * parsed_columns) +
                    column);

            parsed_cells[index] =
                parsed_cell;
        }
    }

    rows_ =
        parsed_rows;

    columns_ =
        parsed_columns;

    cells_ =
        std::move(parsed_cells);

    start_.reset();
    goal_.reset();

    path_.clear();

    return true;
}

void MazeSession::clear()
{
    rows_ = 0;
    columns_ = 0;

    cells_.clear();

    start_.reset();
    goal_.reset();

    path_.clear();
}

bool MazeSession::loaded() const noexcept
{
    return
        rows_ > 0 &&
        columns_ > 0 &&
        !cells_.empty();
}

int MazeSession::rows() const noexcept
{
    return rows_;
}

int MazeSession::columns() const noexcept
{
    return columns_;
}

bool MazeSession::contains(
    const CellCoord & cell) const noexcept
{
    return
        cell.row >= 0 &&
        cell.column >= 0 &&
        cell.row < rows_ &&
        cell.column < columns_;
}

const MazeSession::MazeCell &
MazeSession::cell(
    const CellCoord & coordinate) const
{
    if (!contains(coordinate)) {
        throw std::out_of_range(
            "Maze cell coordinate out of range.");
    }

    return cells_.at(
               indexOf(coordinate));
}

bool MazeSession::setStart(
    const CellCoord & coordinate)
{
    if (!contains(coordinate)) {
        return false;
    }

    start_ =
        coordinate;

    path_.clear();
    path_.push_back(
        coordinate);

    return true;
}

bool MazeSession::setGoal(
    const CellCoord & coordinate)
{
    if (!contains(coordinate)) {
        return false;
    }

    if (
        start_.has_value() &&
        coordinate == start_.value())
    {
        return false;
    }

    goal_ =
        coordinate;

    return true;
}

const std::optional<MazeSession::CellCoord> &
MazeSession::start() const noexcept
{
    return start_;
}

const std::optional<MazeSession::CellCoord> &
MazeSession::goal() const noexcept
{
    return goal_;
}

bool MazeSession::appendPathCell(
    const CellCoord & coordinate,
    std::string * error_message)
{
    if (!loaded()) {
        if (error_message != nullptr) {
            *error_message =
                "Load a maze first.";
        }

        return false;
    }

    if (!start_.has_value()) {
        if (error_message != nullptr) {
            *error_message =
                "Select a start cell first.";
        }

        return false;
    }

    if (!goal_.has_value()) {
        if (error_message != nullptr) {
            *error_message =
                "Select a goal cell first.";
        }

        return false;
    }

    if (!contains(coordinate)) {
        if (error_message != nullptr) {
            *error_message =
                "Selected cell is outside the maze.";
        }

        return false;
    }

    if (path_.empty()) {
        path_.push_back(
            start_.value());
    }

    const CellCoord last =
        path_.back();

    if (coordinate == last) {
        return true;
    }

    /*
     * Convenient manual backtracking:
     *
     * A -> B -> C
     *
     * Clicking B while currently at C:
     *
     * A -> B
     */
    if (
        path_.size() >= 2 &&
        coordinate ==
        path_[path_.size() - 2])
    {
        path_.pop_back();

        return true;
    }

    if (!canMove(last, coordinate)) {
        if (error_message != nullptr) {
            *error_message =
                "Invalid movement: cells must be adjacent "
                "and there must be no wall between them.";
        }

        return false;
    }

    path_.push_back(
        coordinate);

    return true;
}

void MazeSession::undoPathStep()
{
    if (path_.size() <= 1) {
        return;
    }

    path_.pop_back();
}

void MazeSession::clearPath()
{
    path_.clear();

    if (start_.has_value()) {
        path_.push_back(
            start_.value());
    }
}

const std::vector<MazeSession::CellCoord> &
MazeSession::path() const noexcept
{
    return path_;
}

bool MazeSession::isSolved() const noexcept
{
    return
        goal_.has_value() &&
        !path_.empty() &&
        path_.back() ==
        goal_.value();
}

bool MazeSession::canMove(
    const CellCoord & from,
    const CellCoord & to) const noexcept
{
    if (
        !contains(from) ||
        !contains(to))
    {
        return false;
    }

    const int delta_row =
        to.row - from.row;

    const int delta_column =
        to.column - from.column;

    const int manhattan_distance =
        std::abs(delta_row) +
        std::abs(delta_column);

    if (manhattan_distance != 1) {
        return false;
    }

    const MazeCell & source =
        cells_[indexOf(from)];

    const MazeCell & destination =
        cells_[indexOf(to)];

    // Up.
    if (delta_row == -1) {
        return
            !source.top_wall &&
            !destination.bottom_wall;
    }

    // Down.
    if (delta_row == 1) {
        return
            !source.bottom_wall &&
            !destination.top_wall;
    }

    // Left.
    if (delta_column == -1) {
        return
            !source.left_wall &&
            !destination.right_wall;
    }

    // Right.
    if (delta_column == 1) {
        return
            !source.right_wall &&
            !destination.left_wall;
    }

    return false;
}

void MazeSession::setCellSizeMeters(
    const double cell_size_m)
{
    if (
        !std::isfinite(cell_size_m) ||
        cell_size_m <= 0.0)
    {
        throw std::invalid_argument(
            "Cell size must be a finite positive value.");
    }

    cell_size_m_ =
        cell_size_m;
}

double MazeSession::cellSizeMeters() const noexcept
{
    return cell_size_m_;
}

std::vector<MazeSession::MetricTarget>
MazeSession::metricTargets() const
{
    std::vector<MetricTarget> targets;

    if (
        !start_.has_value() ||
        path_.empty())
    {
        return targets;
    }

    targets.reserve(
        path_.size());

    const CellCoord origin =
        start_.value();

    for (const CellCoord & cell_coordinate : path_) {
        MetricTarget target{};

        target.x_m =
            static_cast<double>(
                cell_coordinate.column -
                origin.column) *
            cell_size_m_;

        /*
         * GUI/ASCII rows grow downward.
         * Cartesian Y grows upward.
         */
        target.y_m =
            -static_cast<double>(
                cell_coordinate.row -
                origin.row) *
            cell_size_m_;

        target.source_cell =
            cell_coordinate;

        targets.push_back(
            target);
    }

    return targets;
}

std::size_t MazeSession::indexOf(
    const CellCoord & coordinate) const
{
    return static_cast<std::size_t>(
               (coordinate.row * columns_) +
               coordinate.column);
}

}  // namespace turtlebot3_maze_resolver::services