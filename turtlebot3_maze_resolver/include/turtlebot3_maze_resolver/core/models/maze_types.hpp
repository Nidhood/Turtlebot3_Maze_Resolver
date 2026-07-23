#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace turtlebot3_maze_resolver::models
{

struct CellCoord
{
    int row{0};
    int column{0};

    [[nodiscard]]
    constexpr bool operator==(const CellCoord & other) const noexcept
    {
        return row == other.row &&
               column == other.column;
    }

    [[nodiscard]]
    constexpr bool operator!=(const CellCoord & other) const noexcept
    {
        return !(*this == other);
    }
};

struct MazeCell
{
    bool top_wall{true};
    bool bottom_wall{true};
    bool left_wall{true};
    bool right_wall{true};
};

struct MetricTarget
{
    double x_m{0.0};
    double y_m{0.0};

    CellCoord source_cell{};
};

enum class MazeInteractionMode : std::uint8_t
{
    kNone = 0,
    kSelectStart,
    kSelectGoal,
    kBuildPath
};

}  // namespace turtlebot3_maze_resolver::models