// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "ITheme.h"

/** Built-in default theme: Modrinth-app-inspired dark palette + stylesheet for every dialog. */
class NitruranTheme : public ITheme {
   public:
    QString id() override { return "nitruran"; }
    QString name() override;
    QString tooltip() override { return {}; }
    bool hasStyleSheet() override { return true; }
    QString appStyleSheet() override;
    QString qtTheme() override { return "Fusion"; }
    QPalette colorScheme() override;
    QColor fadeColor() override { return QColor(0x16, 0x18, 0x1c); }
    double fadeAmount() override { return 0.5; }
};
