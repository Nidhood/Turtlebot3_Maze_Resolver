#pragma once

#include <QObject>
#include <QPalette>
#include <QString>

#include "turtlebot3_maze_resolver/core/models/theme_id.hpp"
#include "turtlebot3_maze_resolver/ui/style/theme.hpp"

namespace turtlebot3_maze_resolver::ui::style
{

class ThemeManager final : public QObject
{
    Q_OBJECT

public:
    static ThemeManager & instance();

    void apply(
        models::ThemeId theme_id);

    [[nodiscard]]
    const ThemeSpec &
    currentSpec() const noexcept;

    [[nodiscard]]
    models::ThemeId
    currentId() const noexcept;

signals:
    void themeChanged(
        models::ThemeId theme_id);

private:
    explicit ThemeManager(
        QObject * parent = nullptr);

    [[nodiscard]]
    ThemeSpec makeSpec(
        models::ThemeId theme_id) const;

    [[nodiscard]]
    QPalette buildPalette(
        const ThemeSpec & spec) const;

    [[nodiscard]]
    QString buildStyleSheet(
        const ThemeSpec & spec) const;

private:
    models::ThemeId current_id_{
        models::ThemeId::kTronAres};

    ThemeSpec current_spec_;
};

}  // namespace turtlebot3_maze_resolver::ui::style