#include "turtlebot3_maze_resolver/ui/widgets/maze_widget.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include <algorithm>
#include <cmath>

#include "turtlebot3_maze_resolver/core/services/maze_session.hpp"
#include "turtlebot3_maze_resolver/ui/style/theme_manager.hpp"

namespace turtlebot3_maze_resolver::ui::widgets
{

MazeWidget::MazeWidget(
    QWidget * parent)
    : QWidget(parent)
{
    setMinimumSize(
        500,
        500);

    setMouseTracking(
        true);

    connect(
        &style::ThemeManager::instance(),
        &style::ThemeManager::themeChanged,
        this,
    [this](models::ThemeId) {
        update();
    });
}

void MazeWidget::setSession(
    const services::MazeSession * session)
{
    session_ =
        session;

    update();
}

QSize MazeWidget::minimumSizeHint() const
{
    return {
        500,
        500
    };
}

void MazeWidget::paintEvent(
    QPaintEvent *)
{
    QPainter painter(this);

    painter.setRenderHint(
        QPainter::Antialiasing,
        true);

    const auto & theme =
        style::ThemeManager::instance()
        .currentSpec();

    painter.fillRect(
        rect(),
        theme.background);

    if (
        session_ == nullptr ||
        !session_->loaded())
    {
        painter.setPen(
            theme.text_muted);

        painter.drawText(
            rect(),
            Qt::AlignCenter,
            "Load a .in maze file to begin");

        return;
    }

    const Geometry geometry =
        calculateGeometry();

    const auto & path =
        session_->path();

    /*
     * ------------------------------------------------------------
     * Draw cell backgrounds.
     * ------------------------------------------------------------
     */

    for (
        int row = 0;
        row < session_->rows();
        ++row)
    {
        for (
            int column = 0;
            column < session_->columns();
            ++column)
        {
            QRectF cell_rect(
                geometry.origin_x +
                (column * geometry.cell_size),

                geometry.origin_y +
                (row * geometry.cell_size),

                geometry.cell_size,
                geometry.cell_size);

            QColor fill =
                theme.panel;

            const models::CellCoord coordinate{
                row,
                column};

            const bool in_path =
                std::find(
                    path.begin(),
                    path.end(),
                    coordinate) != path.end();

            if (in_path) {
                fill =
                    theme.accent;

                fill.setAlpha(
                    60);
            }

            if (
                session_->start().has_value() &&
                coordinate ==
                session_->start().value())
            {
                fill =
                    theme.accent_secondary;

                fill.setAlpha(
                    150);
            }

            if (
                session_->goal().has_value() &&
                coordinate ==
                session_->goal().value())
            {
                fill =
                    theme.goal;

                fill.setAlpha(
                    150);
            }

            painter.fillRect(
                cell_rect,
                fill);
        }
    }

    /*
     * ------------------------------------------------------------
     * Draw path center line.
     * ------------------------------------------------------------
     */

    if (path.size() >= 2) {
        QPainterPath painter_path;

        const QPointF first =
            cellCenter(
                path.front().row,
                path.front().column,
                geometry);

        painter_path.moveTo(
            first);

        for (
            std::size_t index = 1;
            index < path.size();
            ++index)
        {
            painter_path.lineTo(
                cellCenter(
                    path[index].row,
                    path[index].column,
                    geometry));
        }

        QPen path_pen(
            theme.accent);

        path_pen.setWidthF(
            std::max(
                3.0,
                geometry.cell_size *
                0.10));

        path_pen.setCapStyle(
            Qt::RoundCap);

        path_pen.setJoinStyle(
            Qt::RoundJoin);

        painter.setPen(
            path_pen);

        painter.drawPath(
            painter_path);
    }

    /*
     * ------------------------------------------------------------
     * Draw maze walls.
     * ------------------------------------------------------------
     */

    QPen wall_pen(
        theme.wall);

    wall_pen.setWidthF(
        std::max(
            2.0,
            geometry.cell_size *
            0.055));

    wall_pen.setCapStyle(
        Qt::SquareCap);

    painter.setPen(
        wall_pen);

    for (
        int row = 0;
        row < session_->rows();
        ++row)
    {
        for (
            int column = 0;
            column < session_->columns();
            ++column)
        {
            const models::CellCoord coordinate{
                row,
                column};

            const auto & maze_cell =
                session_->cell(
                    coordinate);

            const double left =
                geometry.origin_x +
                (column * geometry.cell_size);

            const double top =
                geometry.origin_y +
                (row * geometry.cell_size);

            const double right =
                left +
                geometry.cell_size;

            const double bottom =
                top +
                geometry.cell_size;

            if (maze_cell.top_wall) {
                painter.drawLine(
                    QPointF(left, top),
                    QPointF(right, top));
            }

            if (maze_cell.left_wall) {
                painter.drawLine(
                    QPointF(left, top),
                    QPointF(left, bottom));
            }

            /*
             * Draw only external/right-most duplicates here.
             */
            if (
                column ==
                session_->columns() - 1 &&
                maze_cell.right_wall)
            {
                painter.drawLine(
                    QPointF(right, top),
                    QPointF(right, bottom));
            }

            if (
                row ==
                session_->rows() - 1 &&
                maze_cell.bottom_wall)
            {
                painter.drawLine(
                    QPointF(left, bottom),
                    QPointF(right, bottom));
            }
        }
    }

    /*
     * ------------------------------------------------------------
     * Start / Goal markers.
     * ------------------------------------------------------------
     */

    if (session_->start().has_value()) {
        const auto start =
            session_->start().value();

        const QPointF center =
            cellCenter(
                start.row,
                start.column,
                geometry);

        painter.setPen(
            Qt::NoPen);

        painter.setBrush(
            theme.accent_secondary);

        painter.drawEllipse(
            center,
            geometry.cell_size * 0.18,
            geometry.cell_size * 0.18);

        painter.setPen(
            theme.text);

        painter.drawText(
            QRectF(
                center.x() -
                geometry.cell_size * 0.30,
                center.y() -
                geometry.cell_size * 0.30,

                geometry.cell_size * 0.60,
                geometry.cell_size * 0.60),

            Qt::AlignCenter,
            "S");
    }

    if (session_->goal().has_value()) {
        const auto goal =
            session_->goal().value();

        const QPointF center =
            cellCenter(
                goal.row,
                goal.column,
                geometry);

        painter.setPen(
            Qt::NoPen);

        painter.setBrush(
            theme.goal);

        painter.drawEllipse(
            center,
            geometry.cell_size * 0.18,
            geometry.cell_size * 0.18);

        painter.setPen(
            theme.text);

        painter.drawText(
            QRectF(
                center.x() -
                geometry.cell_size * 0.30,
                center.y() -
                geometry.cell_size * 0.30,

                geometry.cell_size * 0.60,
                geometry.cell_size * 0.60),

            Qt::AlignCenter,
            "G");
    }
}

void MazeWidget::mousePressEvent(
    QMouseEvent * event)
{
    if (
        event->button() !=
        Qt::LeftButton)
    {
        QWidget::mousePressEvent(
            event);

        return;
    }

    if (
        session_ == nullptr ||
        !session_->loaded())
    {
        return;
    }

    const Geometry geometry =
        calculateGeometry();

    const QPointF position =
        event->position();

    const double relative_x =
        position.x() -
        geometry.origin_x;

    const double relative_y =
        position.y() -
        geometry.origin_y;

    if (
        relative_x < 0.0 ||
        relative_y < 0.0 ||
        relative_x >=
        geometry.maze_width ||
        relative_y >=
        geometry.maze_height)
    {
        return;
    }

    const int column =
        static_cast<int>(
            relative_x /
            geometry.cell_size);

    const int row =
        static_cast<int>(
            relative_y /
            geometry.cell_size);

    emit cellClicked(
        row,
        column);
}

MazeWidget::Geometry
MazeWidget::calculateGeometry() const
{
    Geometry geometry{};

    if (
        session_ == nullptr ||
        !session_->loaded())
    {
        return geometry;
    }

    constexpr double kMargin =
        28.0;

    const double available_width =
        std::max(
            1.0,
            static_cast<double>(
                width()) -
            (kMargin * 2.0));

    const double available_height =
        std::max(
            1.0,
            static_cast<double>(
                height()) -
            (kMargin * 2.0));

    const double horizontal_cell =
        available_width /
        static_cast<double>(
            session_->columns());

    const double vertical_cell =
        available_height /
        static_cast<double>(
            session_->rows());

    geometry.cell_size =
        std::max(
            4.0,
            std::min(
                horizontal_cell,
                vertical_cell));

    geometry.maze_width =
        geometry.cell_size *
        static_cast<double>(
            session_->columns());

    geometry.maze_height =
        geometry.cell_size *
        static_cast<double>(
            session_->rows());

    geometry.origin_x =
        (
            static_cast<double>(
                width()) -
            geometry.maze_width) /
        2.0;

    geometry.origin_y =
        (
            static_cast<double>(
                height()) -
            geometry.maze_height) /
        2.0;

    return geometry;
}

QPointF MazeWidget::cellCenter(
    const int row,
    const int column,
    const Geometry & geometry) const
{
    return {
        geometry.origin_x +
        (
            static_cast<double>(
                column) +
            0.5) *
        geometry.cell_size,

        geometry.origin_y +
        (
            static_cast<double>(
                row) +
            0.5) *
        geometry.cell_size
    };
}

}  // namespace turtlebot3_maze_resolver::ui::widgets