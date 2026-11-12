// SPDX-License-Identifier: GPL-3.0-only
// Copies instances / accounts / icons / a few settings from other Prism-family launchers
// (PrismLauncher, mlcmLauncher) into this one. QtCore only, no Application dependency (unit-testable).
#pragma once

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <functional>

struct SyncSource {
    QString id;           // folder name, e.g. "PrismLauncher"
    QString displayName;  // shown in the UI
    QString dataDir;      // e.g. C:/Users/x/AppData/Roaming/PrismLauncher
    QString configFile;   // *.cfg inside dataDir (may be empty)
    QString instanceDir;  // resolved instances folder
    QStringList instances;  // instance folder names (each contains instance.cfg)
    bool hasAccounts = false;
    int iconCount = 0;
};

struct SyncOptions {
    bool instances = true;
    bool accounts = true;
    bool icons = true;
    bool settings = true;
};

struct SyncReport {
    int instancesCopied = 0;
    int instancesSkipped = 0;  // already present here, left untouched
    bool accountsCopied = false;
    bool accountsSkipped = false;  // an accounts file already exists here
    int iconsCopied = 0;
    QMap<QString, QString> settings;  // whitelisted settings read from the source; caller applies them
    QStringList errors;
};

class SyncService {
   public:
    /// roamingDir: the folder that contains every launcher's data folder (e.g. %APPDATA%).
    /// ownDataDir is excluded from the result.
    static QList<SyncSource> detect(const QString& roamingDir, const QString& ownDataDir);

    static SyncReport sync(const SyncSource& source,
                           const QString& destDataDir,
                           const QString& destInstanceDir,
                           const SyncOptions& options,
                           const std::function<void(const QString&)>& progress = {});

    /// Setting keys that are safe to carry over.
    static QStringList settingsWhitelist();

    // exposed for tests
    static QString readIniValue(const QString& cfgPath, const QString& key);
    static bool copyTree(const QString& from, const QString& to, QString* error = nullptr);
};
