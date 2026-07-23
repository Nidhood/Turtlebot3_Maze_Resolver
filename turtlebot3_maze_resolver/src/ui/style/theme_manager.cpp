#include "turtlebot3_maze_resolver/ui/style/theme_manager.hpp"

#include <QApplication>

namespace turtlebot3_maze_resolver::ui::style
{

ThemeManager & ThemeManager::instance()
{
    static ThemeManager instance;

    return instance;
}

ThemeManager::ThemeManager(
    QObject * parent)
    : QObject(parent),
      current_spec_(
          makeSpec(current_id_))
{
}

void ThemeManager::apply(
    const models::ThemeId theme_id)
{
    const ThemeSpec spec =
        makeSpec(theme_id);

    if (qApp != nullptr) {
        qApp->setPalette(
            buildPalette(spec));

        qApp->setStyleSheet(
            buildStyleSheet(spec));
    }

    current_id_ =
        theme_id;

    current_spec_ =
        spec;

    emit themeChanged(
        theme_id);
}

const ThemeSpec &
ThemeManager::currentSpec() const noexcept
{
    return current_spec_;
}

models::ThemeId
ThemeManager::currentId() const noexcept
{
    return current_id_;
}

ThemeSpec ThemeManager::makeSpec(
    const models::ThemeId theme_id) const
{
    switch (theme_id) {
    case models::ThemeId::kLight:
        return {
            .id = theme_id,
            .display_name = "Light",

            .background = QColor("#F1F5F9"),
            .panel = QColor("#FFFFFF"),

            .text = QColor("#0F172A"),
            .text_muted = QColor("#64748B"),

            .accent = QColor("#2563EB"),
            .accent_secondary = QColor("#10B981"),

            .wall = QColor("#0F172A"),
            .goal = QColor("#EF4444")
        };

    case models::ThemeId::kTronAres:
        return {
            .id = theme_id,
            .display_name = "Tron Ares",

            .background = QColor("#050507"),
            .panel = QColor("#0D0D12"),

            .text = QColor("#E6EDF3"),
            .text_muted = QColor("#788694"),

            .accent = QColor("#FF1744"),
            .accent_secondary = QColor("#24C8FF"),

            .wall = QColor("#AAB4C0"),
            .goal = QColor("#FFB020")
        };

    case models::ThemeId::kDark:
    default:
        return {
            .id = models::ThemeId::kDark,
            .display_name = "Dark",

            .background = QColor("#0A0E1A"),
            .panel = QColor("#141B2E"),

            .text = QColor("#E2E8F0"),
            .text_muted = QColor("#94A3B8"),

            .accent = QColor("#60A5FA"),
            .accent_secondary = QColor("#34D399"),

            .wall = QColor("#CBD5E1"),
            .goal = QColor("#FB923C")
        };
    }
}

QPalette ThemeManager::buildPalette(
    const ThemeSpec & spec) const
{
    QPalette palette;

    palette.setColor(
        QPalette::Window,
        spec.background);

    palette.setColor(
        QPalette::WindowText,
        spec.text);

    palette.setColor(
        QPalette::Base,
        spec.panel);

    palette.setColor(
        QPalette::AlternateBase,
        spec.background.lighter(110));

    palette.setColor(
        QPalette::Text,
        spec.text);

    palette.setColor(
        QPalette::Button,
        spec.panel);

    palette.setColor(
        QPalette::ButtonText,
        spec.text);

    palette.setColor(
        QPalette::Highlight,
        spec.accent);

    palette.setColor(
        QPalette::HighlightedText,
        QColor("#FFFFFF"));

    return palette;
}

QString ThemeManager::buildStyleSheet(
    const ThemeSpec & spec) const
{
    return QString(R"(
    * {
      font-family: "Inter", "Segoe UI", "DejaVu Sans", sans-serif;
      font-size: 12px;
    }

    QMainWindow,
    QWidget {
      background-color: %1;
      color: %3;
    }

    QFrame#panel {
      background-color: %2;
      border: 1px solid %4;
      border-radius: 12px;
    }

    QLabel#pageTitle {
      color: %5;
      font-size: 24px;
      font-weight: 700;
    }

    QLabel#pageSubtitle {
      color: %4;
      font-size: 13px;
    }

    QLabel#sectionTitle {
      color: %5;
      font-size: 15px;
      font-weight: 700;
    }

    QLabel#muted {
      color: %4;
    }

    QPushButton {
      background-color: %5;
      color: #FFFFFF;
      border: none;
      border-radius: 8px;
      padding: 9px 14px;
      font-weight: 600;
    }

    QPushButton:hover {
      border: 1px solid %6;
    }

    QPushButton:disabled {
      background-color: %4;
      color: %2;
    }

    QPushButton#secondaryButton {
      background-color: transparent;
      color: %3;
      border: 1px solid %4;
    }

    QPushButton#secondaryButton:hover {
      border-color: %5;
      color: %5;
    }

    QPushButton#dangerButton {
      background-color: #B91C1C;
      color: #FFFFFF;
    }

    QDoubleSpinBox,
    QSpinBox,
    QComboBox,
    QLineEdit {
      background-color: %1;
      color: %3;
      border: 1px solid %4;
      border-radius: 7px;
      padding: 7px 10px;
    }

    QDoubleSpinBox:focus,
    QSpinBox:focus,
    QComboBox:focus,
    QLineEdit:focus {
      border-color: %5;
    }

    QToolBar#topBar {
      background-color: %2;
      border: none;
      border-bottom: 1px solid %4;
      padding: 4px 10px;
    }

    QToolButton#themeButton {
      background-color: transparent;
      color: %3;
      border: none;
      padding: 7px 12px;
      border-radius: 6px;
    }

    QToolButton#themeButton:hover {
      background-color: %1;
      color: %5;
    }

    QMenu {
      background-color: %2;
      color: %3;
      border: 1px solid %4;
    }

    QMenu::item {
      padding: 7px 18px;
    }

    QMenu::item:selected {
      background-color: %5;
      color: #FFFFFF;
    }

    QStatusBar {
      background-color: %2;
      color: %3;
      border-top: 1px solid %4;
    }
  )")
           .arg(
               spec.background.name(),
               spec.panel.name(),
               spec.text.name(),
               spec.text_muted.name(),
               spec.accent.name(),
               spec.accent_secondary.name());
}

}  // namespace turtlebot3_maze_resolver::ui::style