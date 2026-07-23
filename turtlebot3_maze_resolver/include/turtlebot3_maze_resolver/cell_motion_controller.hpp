#pragma once

#include "turtlebot3_maze_resolver/motion_types.hpp"

namespace turtlebot3_maze_resolver {

class CellMotionController {
private:
    ControllerConfig config_{};
    Target2D target_{};
    MotionState state_{MotionState::kIdle};
public:
    explicit CellMotionController( ControllerConfig config );
    void setTarget( const Target2D & target );
    void reset() noexcept;

    [[nodiscard]]
    ControllerOutput update( const Pose2D & current_pose );

    [[nodiscard]]
    MotionState state() const noexcept;

    [[nodiscard]]
    const Target2D & target() const noexcept;
};
} // namespace turtlebot3_maze_resolver