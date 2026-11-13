// SPDX-License-Identifier: GPL-3.0-only
#include "NitruranTheme.h"

#include <QObject>

#include "ui/nitruran/NitruranStyle.h"

QString NitruranTheme::name()
{
    return QStringLiteral("Nitruran");
}

QString NitruranTheme::appStyleSheet()
{
    return Nitruran::appStyleSheet();
}

QPalette NitruranTheme::colorScheme()
{
    using namespace Nitruran;
    QPalette p;
    p.setColor(QPalette::Window, kSurface1);
    p.setColor(QPalette::WindowText, kText);
    p.setColor(QPalette::Base, kSurface2);
    p.setColor(QPalette::AlternateBase, QColor(0x22, 0x24, 0x29));
    p.setColor(QPalette::ToolTipBase, kSurface3);
    p.setColor(QPalette::ToolTipText, kTextPrimary);
    p.setColor(QPalette::Text, kText);
    p.setColor(QPalette::Button, kSurface4);
    p.setColor(QPalette::ButtonText, kTextPrimary);
    p.setColor(QPalette::BrightText, kTextPrimary);
    p.setColor(QPalette::Link, QColor(0x4f, 0x9c, 0xff));
    p.setColor(QPalette::Highlight, kBrand);
    p.setColor(QPalette::HighlightedText, Qt::black);
    p.setColor(QPalette::PlaceholderText, kTextSecondary);
    return fadeInactive(p, fadeAmount(), fadeColor());
}
