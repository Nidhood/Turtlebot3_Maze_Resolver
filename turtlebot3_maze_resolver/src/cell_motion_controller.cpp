#include "turtlebot3_maze_resolver/cell_motion_controller.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace turtlebot3_maze_resolver
{

namespace
{

double normalizeAngle(const double angle_rad) noexcept
{
    return std::atan2(
               std::sin(angle_rad),
               std::cos(angle_rad));
}

double clampSymmetric(
    const double value,
    const double maximum_absolute_value) noexcept
{
    return std::clamp(
               value,
               -maximum_absolute_value,
               maximum_absolute_value);
}

}  // namespace

CellMotionController::CellMotionController(
    ControllerConfig config)
    : config_(std::move(config))
{
    if (config_.kp_linear < 0.0) {
        throw std::invalid_argument(
            "kp_linear must be non-negative");
    }

    if (config_.kp_angular < 0.0) {
        throw std::invalid_argument(
            "kp_angular must be non-negative");
    }

    if (config_.kp_drive_angular < 0.0) {
        throw std::invalid_argument(
            "kp_drive_angular must be non-negative");
    }

    if (config_.max_linear_velocity_mps <= 0.0) {
        throw std::invalid_argument(
            "max_linear_velocity_mps must be greater than zero");
    }

    if (config_.max_angular_velocity_radps <= 0.0) {
        throw std::invalid_argument(
            "max_angular_velocity_radps must be greater than zero");
    }

    if (config_.position_tolerance_m <= 0.0) {
        throw std::invalid_argument(
            "position_tolerance_m must be greater than zero");
    }

    if (config_.heading_tolerance_rad <= 0.0) {
        throw std::invalid_argument(
            "heading_tolerance_rad must be greater than zero");
    }

    if (
        config_.realign_threshold_rad <=
        config_.heading_tolerance_rad)
    {
        throw std::invalid_argument(
            "realign_threshold_rad must be greater than "
            "heading_tolerance_rad");
    }
}

void CellMotionController::setTarget(
    const Target2D & target)
{
    target_ = target;

    state_ = MotionState::kAligning;
}

void CellMotionController::reset() noexcept
{
    state_ = MotionState::kIdle;

    target_ = {};
}

ControllerOutput CellMotionController::update(
    const Pose2D & current_pose)
{
    ControllerOutput output{};

    output.state = state_;

    if (
        state_ == MotionState::kIdle ||
        state_ == MotionState::kSucceeded)
    {
        return output;
    }

    const double error_x =
        target_.x_m - current_pose.x_m;

    const double error_y =
        target_.y_m - current_pose.y_m;

    const double distance_to_target =
        std::hypot(error_x, error_y);

    output.distance_to_target_m =
        distance_to_target;

    // Target reached.
    if (
        distance_to_target <=
        config_.position_tolerance_m)
    {
        state_ = MotionState::kSucceeded;

        output.state = state_;

        return output;
    }

    const double desired_heading =
        std::atan2(error_y, error_x);

    const double heading_error =
        normalizeAngle(
            desired_heading -
            current_pose.yaw_rad);

    output.heading_error_rad =
        heading_error;

    /*
     * STATE 1: ALIGNING
     *
     * The robot rotates in place until it points
     * toward the target.
     */
    if (state_ == MotionState::kAligning) {
        if (
            std::abs(heading_error) >
            config_.heading_tolerance_rad)
        {
            output.command.angular_z_radps =
                clampSymmetric(
                    config_.kp_angular *
                    heading_error,
                    config_.max_angular_velocity_radps);

            output.state = state_;

            return output;
        }

        state_ = MotionState::kDriving;
    }

    /*
     * STATE 2: DRIVING
     *
     * If the robot deviates too much while driving,
     * translation is stopped and alignment is performed again.
     */
    if (state_ == MotionState::kDriving) {
        if (
            std::abs(heading_error) >
            config_.realign_threshold_rad)
        {
            state_ = MotionState::kAligning;

            output.command.linear_x_mps = 0.0;

            output.command.angular_z_radps =
                clampSymmetric(
                    config_.kp_angular *
                    heading_error,
                    config_.max_angular_velocity_radps);

            output.state = state_;

            return output;
        }

        /*
         * Linear proportional controller.
         *
         * The closer we are to the target,
         * the slower the robot moves.
         */
        double linear_velocity =
            config_.kp_linear *
            distance_to_target;

        linear_velocity =
            std::clamp(
                linear_velocity,
                0.0,
                config_.max_linear_velocity_mps);

        /*
         * Reduce forward speed if heading error increases.
         *
         * cos(0) = 1.0
         * perfect alignment -> full requested velocity.
         */
        const double heading_scale =
            std::max(
                0.0,
                std::cos(heading_error));

        linear_velocity *= heading_scale;

        /*
         * Small angular correction while translating.
         */
        const double angular_velocity =
            clampSymmetric(
                config_.kp_drive_angular *
                heading_error,
                config_.max_angular_velocity_radps);

        output.command.linear_x_mps =
            linear_velocity;

        output.command.angular_z_radps =
            angular_velocity;
    }

    output.state = state_;

    return output;
}

MotionState CellMotionController::state() const noexcept
{
    return state_;
}

const Target2D &
CellMotionController::target() const noexcept
{
    return target_;
}

}  // namespace turtlebot3_maze_resolver