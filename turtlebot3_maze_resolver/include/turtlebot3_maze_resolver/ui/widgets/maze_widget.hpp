#pragma once

#include <QPointF>
#include <QWidget>

#include "turtlebot3_maze_resolver/core/models/maze_types.hpp"

namespace turtlebot3_maze_resolver::services
{
class MazeSession;
}

namespace turtlebot3_maze_resolver::ui::widgets
{

class MazeWidget final : public QWidget
{
    Q_OBJECT

public:
    explicit MazeWidget(
        QWidget * parent = nullptr);

    void setSession(
        const services::MazeSession * session);

    [[nodiscard]]
    QSize minimumSizeHint() const override;

signals:
    void cellClicked(
        int row,
        int column);

protected:
    void paintEvent(
        QPaintEvent * event) override;

    void mousePressEvent(
        QMouseEvent * event) override;

private:
    struct Geometry
    {
        double cell_size{0.0};

        double origin_x{0.0};
        double origin_y{0.0};

        double maze_width{0.0};
        double maze_height{0.0};
    };

    [[nodiscard]]
    Geometry calculateGeometry() const;

    [[nodiscard]]
    QPointF cellCenter(
        int row,
        int column,
        const Geometry & geometry) const;

private:
    const services::MazeSession * session_{
        nullptr};
};

}  // namespace turtlebot3_maze_resolver::ui::widgets