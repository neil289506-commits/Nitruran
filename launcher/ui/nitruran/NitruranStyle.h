// SPDX-License-Identifier: GPL-3.0-only
// Design tokens copied from modrinth/code packages/assets/styles/variables.scss (.dark-mode block).
#pragma once
#include <QColor>
#include <QString>

namespace Nitruran {
inline const QColor kSurface1(0x16, 0x18, 0x1c);
inline const QColor kSurface2(0x1d, 0x1f, 0x23);
inline const QColor kSurface3(0x27, 0x29, 0x2e);
inline const QColor kSurface4(0x34, 0x36, 0x3c);
inline const QColor kSurface5(0x42, 0x44, 0x4a);
inline const QColor kText(0xb0, 0xba, 0xc5);
inline const QColor kTextSecondary(0x96, 0xa2, 0xb0);
inline const QColor kTextPrimary(0xff, 0xff, 0xff);
inline const QColor kBrand(0x1b, 0xd9, 0x6a);
inline const QColor kBorder(193, 190, 209, 31);  // rgba(193,190,209,0.12)
constexpr int kRadiusSm = 8;
constexpr int kRadiusMd = 12;

inline QString windowStyleSheet()
{
    return QStringLiteral(R"(
#NitruranRoot { background: #16181c; }
#NitruranSidebar { background: #1d1f23; border-right: 1px solid rgba(193,190,209,0.12); }
#NitruranSidebar QToolButton {
    color: #b0bac5; background: transparent; border: none; border-radius: 12px;
    padding: 8px 4px; min-width: 56px; font-size: 11px;
}
#NitruranSidebar QToolButton:hover { background: #27292e; color: #ffffff; }
#NitruranSidebar QToolButton:checked { background: rgba(27,217,106,0.25); color: #1bd96a; }
#NitruranTitle { color: #ffffff; font-size: 22px; font-weight: 700; }
#NitruranSub { color: #96a2b0; }
QListView#NitruranGrid { background: transparent; border: none; outline: none; }
QScrollBar:vertical, QScrollBar:horizontal { background: transparent; }
QScrollBar:vertical { width: 12px; } QScrollBar:horizontal { height: 12px; }
QScrollBar::handle { background: #42444a; border-radius: 6px; min-height: 30px; min-width: 30px; }
QScrollBar::handle:hover { background: #96a2b0; }
QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }
QMenu { background: #27292e; color: #b0bac5; border: 1px solid rgba(193,190,209,0.12); border-radius: 12px; padding: 6px; }
QMenu::item { padding: 7px 20px; border-radius: 8px; }
QMenu::item:selected { background: rgba(27,217,106,0.25); color: #1bd96a; }
QMenu::separator { height: 1px; background: #34363c; margin: 4px 8px; }
QToolTip { background: #27292e; color: #ffffff; border: 1px solid rgba(193,190,209,0.12); border-radius: 8px; padding: 4px 8px; }
#NitruranPrimary { background: #1bd96a; color: #000000; border: none; border-radius: 12px; padding: 8px 16px; font-weight: 600; }
#NitruranPrimary:hover { background: #42e686; }
)");
}

/// Application-wide stylesheet used by the built-in "Nitruran" theme, so every Prism dialog
/// (settings, new instance, instance window, ...) matches the Nitruran window.
inline QString appStyleSheet()
{
    return QStringLiteral(R"(
QToolTip { background: #27292e; color: #ffffff; border: 1px solid rgba(193,190,209,0.12); border-radius: 8px; padding: 4px 8px; }
QMenu { background: #27292e; color: #b0bac5; border: 1px solid rgba(193,190,209,0.12); border-radius: 12px; padding: 6px; }
QMenu::item { padding: 7px 20px; border-radius: 8px; }
QMenu::item:selected { background: rgba(27,217,106,0.25); color: #1bd96a; }
QMenu::item:disabled { color: #5d6670; }
QMenu::separator { height: 1px; background: #34363c; margin: 4px 8px; }
QMenuBar { background: #1d1f23; color: #b0bac5; }
QMenuBar::item { padding: 6px 12px; border-radius: 8px; background: transparent; }
QMenuBar::item:selected { background: #27292e; }
QPushButton, QToolButton { background: #34363c; color: #ffffff; border: 1px solid rgba(193,190,209,0.12); border-radius: 12px; padding: 6px 14px; }
QPushButton:hover, QToolButton:hover { background: #42444a; }
QPushButton:pressed, QToolButton:pressed { background: #27292e; }
QPushButton:default { background: #1bd96a; color: #000000; border: none; font-weight: 600; }
QPushButton:default:hover { background: #42e686; }
QPushButton:disabled, QToolButton:disabled { background: #222429; color: #6b7682; }
QToolButton:checked { background: rgba(27,217,106,0.25); color: #1bd96a; }
QLineEdit, QPlainTextEdit, QTextEdit, QTextBrowser, QSpinBox, QDoubleSpinBox, QComboBox {
    background: #27292e; color: #ffffff; border: 1px solid rgba(193,190,209,0.12); border-radius: 12px; padding: 6px 10px;
    selection-background-color: #1bd96a; selection-color: #000000; }
QSpinBox::up-button, QSpinBox::down-button, QDoubleSpinBox::up-button, QDoubleSpinBox::down-button { width: 0; border: none; background: transparent; }
QLineEdit:focus, QPlainTextEdit:focus, QTextEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus { border: 1px solid #1bd96a; }
QLineEdit:disabled, QSpinBox:disabled, QComboBox:disabled { background: #222429; color: #6b7682; }
QComboBox QAbstractItemView { background: #27292e; border: 1px solid rgba(193,190,209,0.12); border-radius: 8px;
    selection-background-color: rgba(27,217,106,0.25); selection-color: #1bd96a; }
QListView, QTreeView, QTableView, QListWidget, QTreeWidget, QTableWidget {
    background: #1d1f23; alternate-background-color: #222429; border: 1px solid rgba(193,190,209,0.12); border-radius: 12px; }
QListView::item:selected, QTreeView::item:selected, QTableView::item:selected { background: rgba(27,217,106,0.25); color: #1bd96a; }
QListView::item:hover, QTreeView::item:hover { background: #27292e; }
QHeaderView::section { background: #27292e; color: #96a2b0; border: none; padding: 6px 10px; }
QTabWidget::pane { border: 1px solid rgba(193,190,209,0.12); border-radius: 12px; top: -1px; }
QTabBar::tab { background: transparent; color: #b0bac5; padding: 8px 16px; border-radius: 12px; margin: 2px; }
QTabBar::tab:hover { background: #27292e; }
QTabBar::tab:selected { background: rgba(27,217,106,0.25); color: #1bd96a; }
QGroupBox { background: #1d1f23; border: 1px solid rgba(193,190,209,0.12); border-radius: 12px; margin-top: 14px; padding-top: 10px; }
QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; color: #ffffff; }
QCheckBox::indicator, QRadioButton::indicator { width: 16px; height: 16px; border: 1px solid #42444a; background: #27292e; }
QCheckBox::indicator { border-radius: 4px; }
QRadioButton::indicator { border-radius: 8px; }
QCheckBox::indicator:checked, QRadioButton::indicator:checked { background: #1bd96a; border-color: #1bd96a; }
QProgressBar { background: #27292e; border: none; border-radius: 8px; text-align: center; color: #ffffff; }
QProgressBar::chunk { background: #1bd96a; border-radius: 8px; }
QScrollBar:vertical { background: transparent; width: 12px; margin: 0; }
QScrollBar::handle:vertical { background: #42444a; border-radius: 6px; min-height: 30px; }
QScrollBar::handle:vertical:hover { background: #96a2b0; }
QScrollBar:horizontal { background: transparent; height: 12px; margin: 0; }
QScrollBar::handle:horizontal { background: #42444a; border-radius: 6px; min-width: 30px; }
QScrollBar::handle:horizontal:hover { background: #96a2b0; }
QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }
QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }
QToolBar { background: #1d1f23; border: none; spacing: 6px; padding: 6px; }
QStatusBar { background: #16181c; color: #96a2b0; }
QSplitter::handle { background: #27292e; }
)");
}
}  // namespace Nitruran
