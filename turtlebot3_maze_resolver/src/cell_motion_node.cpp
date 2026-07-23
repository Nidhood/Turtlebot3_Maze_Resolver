#include "turtlebot3_maze_resolver/cell_motion_node.hpp"

#include <cmath>
#include <functional>
#include <stdexcept>

#include "turtlebot3_maze_resolver/constants.hpp"

namespace turtlebot3_maze_resolver
{

CellMotionNode::CellMotionNode()
    : rclcpp::Node(constants::kNodeName)
{
    /*
     * ============================================================
     * Target parameters
     * ============================================================
     *
     * Coordinates are always expressed in meters.
     *
     * target_frame = "start"
     *   Coordinates are relative to the robot's initial pose.
     *
     * target_frame = "odom"
     *   Coordinates are absolute in the odometry frame.
     */

    requested_target_x_m_ =
        this->declare_parameter<double>(
            "target_x",
            0.0);

    requested_target_y_m_ =
        this->declare_parameter<double>(
            "target_y",
            0.0);

    target_frame_ =
        this->declare_parameter<std::string>(
            "target_frame",
            constants::kTargetFrameStart);

    if (
        target_frame_ != constants::kTargetFrameStart &&
        target_frame_ != constants::kTargetFrameOdom)
    {
        throw std::invalid_argument(
            "target_frame must be either 'start' or 'odom'");
    }

    if (
        !std::isfinite(requested_target_x_m_) ||
        !std::isfinite(requested_target_y_m_))
    {
        throw std::invalid_argument(
            "target_x and target_y must contain finite values");
    }

    /*
     * ============================================================
     * ROS interface parameters
     * ============================================================
     */

    odom_topic_ =
        this->declare_parameter<std::string>(
            "odom_topic",
            constants::kDefaultOdomTopic);

    cmd_vel_topic_ =
        this->declare_parameter<std::string>(
            "cmd_vel_topic",
            constants::kDefaultCmdVelTopic);

    command_frame_id_ =
        this->declare_parameter<std::string>(
            "command_frame_id",
            constants::kDefaultCommandFrame);

    use_stamped_cmd_vel_ =
        this->declare_parameter<bool>(
            "use_stamped_cmd_vel",
            true);

    /*
     * ============================================================
     * Timing parameters
     * ============================================================
     */

    control_frequency_hz_ =
        this->declare_parameter<double>(
            "control_frequency_hz",
            50.0);

    motion_timeout_sec_ =
        this->declare_parameter<double>(
            "motion_timeout_sec",
            30.0);

    odom_timeout_sec_ =
        this->declare_parameter<double>(
            "odom_timeout_sec",
            1.0);

    if (control_frequency_hz_ <= 0.0) {
        throw std::invalid_argument(
            "control_frequency_hz must be greater than zero");
    }

    if (motion_timeout_sec_ <= 0.0) {
        throw std::invalid_argument(
            "motion_timeout_sec must be greater than zero");
    }

    if (odom_timeout_sec_ <= 0.0) {
        throw std::invalid_argument(
            "odom_timeout_sec must be greater than zero");
    }

    /*
     * ============================================================
     * Controller parameters
     * ============================================================
     */

    ControllerConfig controller_config{};

    controller_config.kp_linear =
        this->declare_parameter<double>(
            "kp_linear",
            controller_config.kp_linear);

    controller_config.kp_angular =
        this->declare_parameter<double>(
            "kp_angular",
            controller_config.kp_angular);

    controller_config.kp_drive_angular =
        this->declare_parameter<double>(
            "kp_drive_angular",
            controller_config.kp_drive_angular);

    controller_config.max_linear_velocity_mps =
        this->declare_parameter<double>(
            "max_linear_velocity",
            controller_config.max_linear_velocity_mps);

    controller_config.max_angular_velocity_radps =
        this->declare_parameter<double>(
            "max_angular_velocity",
            controller_config.max_angular_velocity_radps);

    controller_config.position_tolerance_m =
        this->declare_parameter<double>(
            "position_tolerance",
            controller_config.position_tolerance_m);

    controller_config.heading_tolerance_rad =
        this->declare_parameter<double>(
            "heading_tolerance",
            controller_config.heading_tolerance_rad);

    controller_config.realign_threshold_rad =
        this->declare_parameter<double>(
            "realign_threshold",
            controller_config.realign_threshold_rad);

    controller_ =
        std::make_unique<CellMotionController>(
            controller_config);

    /*
     * ============================================================
     * cmd_vel publisher
     * ============================================================
     *
     * Current TurtleBot3 Gazebo uses TwistStamped by default.
     *
     * The parameter allows us to switch later to Twist if needed.
     */

    if (use_stamped_cmd_vel_) {
        twist_stamped_publisher_ =
            this->create_publisher<
            geometry_msgs::msg::TwistStamped>(
                cmd_vel_topic_,
                rclcpp::QoS(10));
    } else {
        twist_publisher_ =
            this->create_publisher<
            geometry_msgs::msg::Twist>(
                cmd_vel_topic_,
                rclcpp::QoS(10));
    }

    /*
     * ============================================================
     * Odometry subscriber
     * ============================================================
     */

    odometry_subscription_ =
        this->create_subscription<
        nav_msgs::msg::Odometry>(
            odom_topic_,
            rclcpp::SensorDataQoS(),
            std::bind(
                &CellMotionNode::odometryCallback,
                this,
                std::placeholders::_1));

    /*
     * ============================================================
     * Control timer
     * ============================================================
     */

    const auto control_period =
        std::chrono::duration_cast<
        std::chrono::nanoseconds>(
            std::chrono::duration<double>(
                1.0 / control_frequency_hz_));

    control_timer_ =
        this->create_wall_timer(
            control_period,
            std::bind(
                &CellMotionNode::controlLoop,
                this));

    RCLCPP_INFO(
        this->get_logger(),
        "Cell motion node initialized");

    RCLCPP_INFO(
        this->get_logger(),
        "Requested target: x=%.4f m, y=%.4f m, frame='%s'",
        requested_target_x_m_,
        requested_target_y_m_,
        target_frame_.c_str());

    RCLCPP_INFO(
        this->get_logger(),
        "Interfaces: odom='%s', cmd_vel='%s', stamped=%s",
        odom_topic_.c_str(),
        cmd_vel_topic_.c_str(),
        use_stamped_cmd_vel_ ? "true" : "false");

    maze_target_subscription_ =
        this->create_subscription<
        geometry_msgs::msg::PointStamped>(
            "maze/target",
            rclcpp::QoS(10),

            std::bind(
                &CellMotionNode::mazeTargetCallback,
                this,
                std::placeholders::_1));

    maze_stop_subscription_ =
        this->create_subscription<
        std_msgs::msg::Empty>(
            "maze/stop",
            rclcpp::QoS(10),

            std::bind(
                &CellMotionNode::mazeStopCallback,
                this,
                std::placeholders::_1));

    motion_state_publisher_ =
        this->create_publisher<
        std_msgs::msg::UInt8>(
            "maze/motion_state",
            rclcpp::QoS(10));
}

CellMotionNode::~CellMotionNode()
{
    if (rclcpp::ok()) {
        publishStop();
    }
}

void CellMotionNode::odometryCallback(
    const nav_msgs::msg::Odometry::SharedPtr message)
{
    latest_pose_.x_m =
        message->pose.pose.position.x;

    latest_pose_.y_m =
        message->pose.pose.position.y;

    const auto & orientation =
        message->pose.pose.orientation;

    latest_pose_.yaw_rad =
        quaternionToYaw(
            orientation.x,
            orientation.y,
            orientation.z,
            orientation.w);

    if (!start_pose_initialized_) {
        start_pose_ =
            latest_pose_;

        start_pose_initialized_ =
            true;

        RCLCPP_INFO(
            this->get_logger(),
            "Maze start frame initialized: "
            "x=%.3f, y=%.3f, yaw=%.3f",
            start_pose_.x_m,
            start_pose_.y_m,
            start_pose_.yaw_rad);
    }

    has_odom_ = true;

    last_odom_time_ =
        std::chrono::steady_clock::now();

    /*
     * The first odometry sample establishes the reference
     * coordinate system when target_frame = "start".
     */
    if (!target_initialized_) {
        initializeTarget(latest_pose_);
    }
}

void CellMotionNode::initializeTarget(
    const Pose2D & initial_pose)
{
    Target2D resolved_target{};

    if (
        target_frame_ ==
        constants::kTargetFrameStart)
    {
        /*
         * Transform target coordinates from the initial robot/maze
         * frame into the odometry frame.
         *
         * [x_odom]   [x0]   [ cos(yaw) -sin(yaw)] [x_target]
         * [y_odom] = [y0] + [ sin(yaw)  cos(yaw)] [y_target]
         */

        const double cos_yaw =
            std::cos(initial_pose.yaw_rad);

        const double sin_yaw =
            std::sin(initial_pose.yaw_rad);

        resolved_target.x_m =
            initial_pose.x_m +
            cos_yaw * requested_target_x_m_ -
            sin_yaw * requested_target_y_m_;

        resolved_target.y_m =
            initial_pose.y_m +
            sin_yaw * requested_target_x_m_ +
            cos_yaw * requested_target_y_m_;
    } else {
        resolved_target.x_m =
            requested_target_x_m_;

        resolved_target.y_m =
            requested_target_y_m_;
    }

    controller_->setTarget(
        resolved_target);

    previous_state_ =
        controller_->state();

    target_initialized_ = true;

    motion_start_time_ =
        std::chrono::steady_clock::now();

    RCLCPP_INFO(
        this->get_logger(),
        "Initial pose: x=%.4f m, y=%.4f m, yaw=%.4f rad",
        initial_pose.x_m,
        initial_pose.y_m,
        initial_pose.yaw_rad);

    RCLCPP_INFO(
        this->get_logger(),
        "Resolved odom target: x=%.4f m, y=%.4f m",
        resolved_target.x_m,
        resolved_target.y_m);
}

void CellMotionNode::controlLoop()
{
    /*
     * Do nothing until odometry is available.
     */
    if (!has_odom_) {
        RCLCPP_WARN_THROTTLE(
            this->get_logger(),
            *this->get_clock(),
            2000,
            "Waiting for odometry...");

        return;
    }

    if (
        !target_initialized_ ||
        motion_finished_)
    {
        return;
    }

    const auto current_time =
        std::chrono::steady_clock::now();

    /*
     * ============================================================
     * Odometry watchdog
     * ============================================================
     *
     * Never continue moving if odometry stops arriving.
     */

    const double odom_age_sec =
        std::chrono::duration<double>(
            current_time -
            last_odom_time_).count();

    if (odom_age_sec > odom_timeout_sec_) {
        publishStop();

        RCLCPP_WARN_THROTTLE(
            this->get_logger(),
            *this->get_clock(),
            2000,
            "Odometry timeout. Robot stopped.");
        publishMotionState(4);
        return;
    }

    /*
     * ============================================================
     * Global motion timeout
     * ============================================================
     */

    const double elapsed_sec =
        std::chrono::duration<double>(
            current_time -
            motion_start_time_).count();

    if (elapsed_sec > motion_timeout_sec_) {
        publishStop();

        motion_finished_ = true;

        RCLCPP_ERROR(
            this->get_logger(),
            "Motion aborted: timeout after %.2f seconds",
            elapsed_sec);

        return;
    }

    /*
     * ============================================================
     * Run pure controller
     * ============================================================
     */

    const ControllerOutput output =
        controller_->update(
            latest_pose_);

    if (output.state != previous_state_) {
        RCLCPP_INFO(
            this->get_logger(),
            "Motion state: %s -> %s",
            motionStateName(previous_state_),
            motionStateName(output.state));

        previous_state_ =
            output.state;

        switch (output.state) {
        case MotionState::kIdle:
            publishMotionState(0);
            break;

        case MotionState::kAligning:
            publishMotionState(1);
            break;

        case MotionState::kDriving:
            publishMotionState(2);
            break;

        case MotionState::kSucceeded:
            publishMotionState(3);
            break;
        }
    }

    publishCommand(
        output.command);

    /*
     * ============================================================
     * Target reached
     * ============================================================
     */

    if (
        output.state ==
        MotionState::kSucceeded)
    {
        publishStop();

        motion_finished_ = true;

        RCLCPP_INFO(
            this->get_logger(),
            "Target reached successfully.");

        RCLCPP_INFO(
            this->get_logger(),
            "Final pose: x=%.4f m, y=%.4f m, yaw=%.4f rad",
            latest_pose_.x_m,
            latest_pose_.y_m,
            latest_pose_.yaw_rad);

        RCLCPP_INFO(
            this->get_logger(),
            "Final position error: %.4f m",
            output.distance_to_target_m);
    }
}

void CellMotionNode::publishCommand(
    const VelocityCommand & command)
{
    if (use_stamped_cmd_vel_) {
        geometry_msgs::msg::TwistStamped message;

        message.header.stamp =
            this->now();

        message.header.frame_id =
            command_frame_id_;

        message.twist.linear.x =
            command.linear_x_mps;

        message.twist.angular.z =
            command.angular_z_radps;

        twist_stamped_publisher_->publish(
            message);

        return;
    }

    geometry_msgs::msg::Twist message;

    message.linear.x =
        command.linear_x_mps;

    message.angular.z =
        command.angular_z_radps;

    twist_publisher_->publish(
        message);
}

void CellMotionNode::publishStop()
{
    publishCommand(
        VelocityCommand{});
}

double CellMotionNode::quaternionToYaw(
    const double x,
    const double y,
    const double z,
    const double w) noexcept
{
    /*
     * Quaternion -> Euler yaw.
     *
     * We only need rotation around Z for a differential-drive robot
     * moving on a planar surface.
     */

    const double sin_yaw =
        2.0 *
        ((w * z) + (x * y));

    const double cos_yaw =
        1.0 -
        2.0 *
        ((y * y) + (z * z));

    return std::atan2(
               sin_yaw,
               cos_yaw);
}

Target2D CellMotionNode::
resolveStartFrameTarget(
    const double x_m,
    const double y_m) const
{
    Target2D target{};

    const double cos_yaw =
        std::cos(
            start_pose_.yaw_rad);

    const double sin_yaw =
        std::sin(
            start_pose_.yaw_rad);

    target.x_m =
        start_pose_.x_m +
        cos_yaw * x_m -
        sin_yaw * y_m;

    target.y_m =
        start_pose_.y_m +
        sin_yaw * x_m +
        cos_yaw * y_m;

    return target;
}

void CellMotionNode::
mazeTargetCallback(
    const geometry_msgs::msg::
    PointStamped::SharedPtr message)
{
    if (!start_pose_initialized_) {
        RCLCPP_WARN(
            this->get_logger(),
            "Cannot accept maze target: "
            "odometry/start frame not initialized.");

        return;
    }

    if (
        !std::isfinite(
            message->point.x) ||
        !std::isfinite(
            message->point.y))
    {
        RCLCPP_ERROR(
            this->get_logger(),
            "Rejected non-finite maze target.");

        return;
    }

    Target2D target{};

    if (
        message->header.frame_id.empty() ||
        message->header.frame_id ==
        "start")
    {
        target =
            resolveStartFrameTarget(
                message->point.x,
                message->point.y);
    } else if (
        message->header.frame_id ==
        "odom")
    {
        target.x_m =
            message->point.x;

        target.y_m =
            message->point.y;
    } else {
        RCLCPP_ERROR(
            this->get_logger(),
            "Unsupported target frame: '%s'",
            message->header.frame_id.c_str());

        return;
    }

    controller_->setTarget(
        target);

    target_initialized_ =
        true;

    motion_finished_ =
        false;

    previous_state_ =
        controller_->state();

    motion_start_time_ =
        std::chrono::steady_clock::now();

    publishMotionState(
        1);

    RCLCPP_INFO(
        this->get_logger(),
        "New maze target received: "
        "maze=(%.3f, %.3f), "
        "odom=(%.3f, %.3f)",
        message->point.x,
        message->point.y,
        target.x_m,
        target.y_m);
}

void CellMotionNode::
mazeStopCallback(
    const std_msgs::msg::Empty::
    SharedPtr)
{
    publishStop();

    controller_->reset();

    target_initialized_ =
        false;

    motion_finished_ =
        true;

    publishMotionState(
        4);

    RCLCPP_WARN(
        this->get_logger(),
        "Maze execution stop requested.");
}

void CellMotionNode::
publishMotionState(
    const std::uint8_t state)
{
    if (!motion_state_publisher_) {
        return;
    }

    std_msgs::msg::UInt8 message;

    message.data =
        state;

    motion_state_publisher_->publish(
        message);
}

}  // namespace turtlebot3_maze_resolver