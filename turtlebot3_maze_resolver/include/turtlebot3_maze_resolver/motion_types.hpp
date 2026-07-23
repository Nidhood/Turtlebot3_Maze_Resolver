#pragma once

#include <cstdint>

namespace turtlebot3_maze_resolver {

enum class MotionState : std::uint8_t {
    kIdle = 0,
    kAligning,
    kDriving,
    kSucceeded
};

struct Pose2D {
    double x_m{0.0};
    double y_m{0.0};
    double yaw_rad{0.0};
};

struct Target2D {
    double x_m{0.0};
    double y_m{0.0};
};

struct VelocityCommand {
    double linear_x_mps{0.0};
    double angular_z_radps{0.0};
};

struct ControllerConfig {
    double kp_linear{1.20};
    double kp_angular{2.50};
    double kp_drive_angular{2.00};
    double max_linear_velocity_mps{0.12};
    double max_angular_velocity_radps{1.00};
    double position_tolerance_m{0.005};
    double heading_tolerance_rad{0.03};
    double realign_threshold_rad{0.15};
};

struct ControllerOutput {
    VelocityCommand command{};
    MotionState state{MotionState::kIdle};
    double distance_to_target_m{0.0};
    double heading_error_rad{0.0};
};

inline constexpr const char * motionStateName( const MotionState state ) noexcept {
    switch ( state ) {
    case MotionState::kIdle:
        return "IDLE";
    case MotionState::kAligning:
        return "ALIGNING";
    case MotionState::kDriving:
        return "DRIVING";
    case MotionState::kSucceeded:
        return "SUCCEEDED";
    default:
        return "UNKNOWN";
    }

}


} // namespace turtlebot3_maze_resolver