#pragma once

#include <optional>
#include <string>
#include <vector>

#include "turtlebot3_maze_resolver/core/models/maze_types.hpp"

namespace turtlebot3_maze_resolver::services
{

class MazeSession
{
public:
    using CellCoord =
        turtlebot3_maze_resolver::models::CellCoord;

    using MazeCell =
        turtlebot3_maze_resolver::models::MazeCell;

    using MetricTarget =
        turtlebot3_maze_resolver::models::MetricTarget;

    bool loadFromFile(
        const std::string & file_path,
        std::string * error_message = nullptr);

    void clear();

    [[nodiscard]]
    bool loaded() const noexcept;

    [[nodiscard]]
    int rows() const noexcept;

    [[nodiscard]]
    int columns() const noexcept;

    [[nodiscard]]
    bool contains(
        const CellCoord & cell) const noexcept;

    [[nodiscard]]
    const MazeCell & cell(
        const CellCoord & coordinate) const;

    bool setStart(
        const CellCoord & coordinate);

    bool setGoal(
        const CellCoord & coordinate);

    [[nodiscard]]
    const std::optional<CellCoord> &
    start() const noexcept;

    [[nodiscard]]
    const std::optional<CellCoord> &
    goal() const noexcept;

    bool appendPathCell(
        const CellCoord & coordinate,
        std::string * error_message = nullptr);

    void undoPathStep();

    void clearPath();

    [[nodiscard]]
    const std::vector<CellCoord> &
    path() const noexcept;

    [[nodiscard]]
    bool isSolved() const noexcept;

    [[nodiscard]]
    bool canMove(
        const CellCoord & from,
        const CellCoord & to) const noexcept;

    void setCellSizeMeters(
        double cell_size_m);

    [[nodiscard]]
    double cellSizeMeters() const noexcept;

    [[nodiscard]]
    std::vector<MetricTarget>
    metricTargets() const;

private:
    [[nodiscard]]
    std::size_t indexOf(
        const CellCoord & coordinate) const;

private:
    int rows_{0};
    int columns_{0};

    double cell_size_m_{0.50};

    std::vector<MazeCell> cells_;

    std::optional<CellCoord> start_;
    std::optional<CellCoord> goal_;

    std::vector<CellCoord> path_;
};

}  // namespace turtlebot3_maze_resolver::services