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
}  // namespace Nitruran
