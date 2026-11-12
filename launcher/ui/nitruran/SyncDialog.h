// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <QDialog>

#include "SyncService.h"

class QCheckBox;
class QLabel;
class QListWidget;
class QPushButton;

/** "Sync from other launchers": pick what to copy from PrismLauncher / mlcmLauncher. */
class SyncDialog : public QDialog {
    Q_OBJECT
   public:
    explicit SyncDialog(QWidget* parent = nullptr);
    /// Sources found in the folder next to our own data folder.
    static QList<SyncSource> detectSources();

   private slots:
    void startSync();

   private:
    QList<SyncSource> m_sources;
    QListWidget* m_list = nullptr;
    QCheckBox* m_instances = nullptr;
    QCheckBox* m_accounts = nullptr;
    QCheckBox* m_icons = nullptr;
    QCheckBox* m_settings = nullptr;
    QLabel* m_status = nullptr;
    QPushButton* m_syncBtn = nullptr;
};
