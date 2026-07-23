#include <QApplication>

#include <rclcpp/rclcpp.hpp>

#include "turtlebot3_maze_resolver/app/app_context.hpp"
#include "turtlebot3_maze_resolver/app/ros_qt_bridge.hpp"
#include "turtlebot3_maze_resolver/core/models/theme_id.hpp"
#include "turtlebot3_maze_resolver/ui/style/theme_manager.hpp"

int main( int argc, char * argv[] ) {

    // 1. Qt first (safe argv ownership & GUI prerequisites):
    QApplication application( argc, argv );
    QApplication::setApplicationName("TurtleBot3 Maze Resolver");
    QApplication::setOrganizationName("Nidhood");

    // 2. ROS after Qt:
    rclcpp::init( argc, argv );

    // 3. Theme:
    turtlebot3_maze_resolver::ui::style::ThemeManager::instance()
    .apply( turtlebot3_maze_resolver::models::ThemeId::kTronAres );

    // 4. App context:
    turtlebot3_maze_resolver::app::AppContext context;
    context.mainWindow().show();

    // 5. Bridge ROS <-> Qt lifetime:
    turtlebot3_maze_resolver::
    app::RosQtBridge ros_qt_bridge( application,context.node() );

    // 6. Qt loop:
    return application.exec();
}