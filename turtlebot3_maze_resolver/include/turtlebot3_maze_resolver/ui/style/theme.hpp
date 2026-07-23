#pragma once

#include <QColor>
#include <QString>

#include "turtlebot3_maze_resolver/core/models/theme_id.hpp"

namespace turtlebot3_maze_resolver::ui::style
{

struct ThemeSpec
{
    models::ThemeId id{models::ThemeId::kDark};

    QString display_name;

    QColor background;
    QColor panel;

    QColor text;
    QColor text_muted;

    QColor accent;
    QColor accent_secondary;

    QColor wall;
    QColor goal;
};

}  // namespace turtlebot3_maze_resolver::ui::style