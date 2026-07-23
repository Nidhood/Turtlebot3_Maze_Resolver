#pragma once

namespace turtlebot3_maze_resolver::constants {

inline constexpr char kNodeName[] = "cell_motion_node";
inline constexpr char kDefaultOdomTopic[] = "odom";
inline constexpr char kDefaultCmdVelTopic[] = "cmd_vel";
inline constexpr char kDefaultCommandFrame[] = "base_link";
inline constexpr char kTargetFrameStart[] = "start";
inline constexpr char kTargetFrameOdom[] = "odom";
inline constexpr double kPi = 3.141592653589793238462643383279502884;

} // namespace turtlebot3_maze_resolver::constants