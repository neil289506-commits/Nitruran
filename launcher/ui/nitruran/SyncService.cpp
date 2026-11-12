// SPDX-License-Identifier: GPL-3.0-only
#include "SyncService.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

namespace {
const QStringList kLauncherFolders = { QStringLiteral("PrismLauncher"), QStringLiteral("mlcmLauncher") };

QString findConfig(const QString& dataDir)
{
    const QStringList names = { QStringLiteral("prismlauncher.cfg"), QStringLiteral("mlcmlauncher.cfg") };
    for (const auto& n : names)
        if (QFileInfo::exists(QDir(dataDir).filePath(n)))
            return QDir(dataDir).filePath(n);
    // any other *.cfg except playtime.cfg
    const auto cfgs = QDir(dataDir).entryInfoList({ QStringLiteral("*.cfg") }, QDir::Files);
    for (const auto& fi : cfgs)
        if (fi.fileName().compare(QStringLiteral("playtime.cfg"), Qt::CaseInsensitive) != 0)
            return fi.absoluteFilePath();
    return {};
}

QJsonObject readJson(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return {};
    return QJsonDocument::fromJson(f.readAll()).object();
}

bool writeJson(const QString& path, const QJsonObject& obj)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    f.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    return true;
}
}  // namespace

QStringList SyncService::settingsWhitelist()
{
    return { QStringLiteral("MinMemAlloc"),    QStringLiteral("MaxMemAlloc"),     QStringLiteral("JvmArgs"),
             QStringLiteral("JavaPath"),       QStringLiteral("Language"),        QStringLiteral("ConsoleFont"),
             QStringLiteral("ShowConsole"),    QStringLiteral("AutoCloseConsole") };
}

QString SyncService::readIniValue(const QString& cfgPath, const QString& key)
{
    QFile f(cfgPath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};
    QTextStream in(&f);
    const QString prefix = key + QLatin1Char('=');
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.startsWith(prefix)) {
            QString v = line.mid(prefix.size()).trimmed();
            if (v.size() >= 2 && v.startsWith(QLatin1Char('"')) && v.endsWith(QLatin1Char('"')))
                v = v.mid(1, v.size() - 2);
            return v;
        }
    }
    return {};
}

QList<SyncSource> SyncService::detect(const QString& roamingDir, const QString& ownDataDir)
{
    QList<SyncSource> out;
    const QString own = QDir::cleanPath(QDir(ownDataDir).absolutePath());
    for (const auto& folder : kLauncherFolders) {
        const QString dir = QDir::cleanPath(QDir(roamingDir).filePath(folder));
        if (!QFileInfo(dir).isDir() || QString::compare(dir, own, Qt::CaseInsensitive) == 0)
            continue;

        SyncSource s;
        s.id = folder;
        s.displayName = folder == QStringLiteral("PrismLauncher") ? QStringLiteral("Prism Launcher") : folder;
        s.dataDir = dir;
        s.configFile = findConfig(dir);

        QString inst = s.configFile.isEmpty() ? QString() : readIniValue(s.configFile, QStringLiteral("InstanceDir"));
        if (inst.isEmpty())
            inst = QStringLiteral("instances");
        s.instanceDir = QDir::isAbsolutePath(inst) ? QDir::cleanPath(inst) : QDir(dir).filePath(inst);

        const auto subdirs = QDir(s.instanceDir).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const auto& d : subdirs) {
            if (d.fileName().startsWith(QLatin1Char('.')) || d.fileName().startsWith(QLatin1Char('_')))
                continue;
            if (QFileInfo::exists(QDir(d.absoluteFilePath()).filePath(QStringLiteral("instance.cfg"))))
                s.instances << d.fileName();
        }
        s.hasAccounts = QFileInfo::exists(QDir(dir).filePath(QStringLiteral("accounts.json")));
        const QString icons = QDir(dir).filePath(QStringLiteral("icons"));
        s.iconCount = QDir(icons).entryList(QDir::Files).size();

        if (!s.instances.isEmpty() || s.hasAccounts || s.iconCount > 0 || !s.configFile.isEmpty())
            out << s;
    }
    return out;
}

bool SyncService::copyTree(const QString& from, const QString& to, QString* error)
{
    const QDir src(from);
    if (!src.exists()) {
        if (error)
            *error = QStringLiteral("missing source: ") + from;
        return false;
    }
    QDir().mkpath(to);
    QDirIterator it(from, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        const QString rel = src.relativeFilePath(it.filePath());
        const QString target = QDir(to).filePath(rel);
        if (it.fileInfo().isDir()) {
            QDir().mkpath(target);
        } else {
            QDir().mkpath(QFileInfo(target).absolutePath());
            if (QFile::exists(target))
                QFile::remove(target);
            if (!QFile::copy(it.filePath(), target)) {
                if (error)
                    *error = QStringLiteral("cannot copy ") + it.filePath();
                return false;
            }
        }
    }
    return true;
}

SyncReport SyncService::sync(const SyncSource& source,
                             const QString& destDataDir,
                             const QString& destInstanceDir,
                             const SyncOptions& options,
                             const std::function<void(const QString&)>& progress)
{
    SyncReport r;
    auto say = [&](const QString& m) {
        if (progress)
            progress(m);
    };

    QStringList copiedIds;
    if (options.instances) {
        QDir().mkpath(destInstanceDir);
        for (const auto& name : source.instances) {
            const QString target = QDir(destInstanceDir).filePath(name);
            if (QFileInfo::exists(target)) {
                ++r.instancesSkipped;  // never overwrite what's already here
                continue;
            }
            say(QStringLiteral("Copying instance %1").arg(name));
            QString err;
            if (copyTree(QDir(source.instanceDir).filePath(name), target, &err)) {
                ++r.instancesCopied;
                copiedIds << name;
            } else {
                r.errors << QStringLiteral("%1: %2").arg(name, err);
                QDir(target).removeRecursively();  // don't leave a half-copied instance behind
            }
        }
        // merge groups for the instances we copied
        if (!copiedIds.isEmpty()) {
            const QString srcFile = QDir(source.instanceDir).filePath(QStringLiteral("instgroups.json"));
            const QString dstFile = QDir(destInstanceDir).filePath(QStringLiteral("instgroups.json"));
            QJsonObject src = readJson(srcFile);
            QJsonObject dst = readJson(dstFile);
            if (!src.isEmpty()) {
                QJsonObject dstGroups = dst.value(QStringLiteral("groups")).toObject();
                const QJsonObject srcGroups = src.value(QStringLiteral("groups")).toObject();
                for (auto it = srcGroups.begin(); it != srcGroups.end(); ++it) {
                    const QJsonObject g = it.value().toObject();
                    QJsonArray add;
                    for (const auto& v : g.value(QStringLiteral("instances")).toArray())
                        if (copiedIds.contains(v.toString()))
                            add.append(v);
                    if (add.isEmpty())
                        continue;
                    QJsonObject dg = dstGroups.value(it.key()).toObject();
                    QJsonArray list = dg.value(QStringLiteral("instances")).toArray();
                    for (const auto& v : add)
                        if (!list.contains(v))
                            list.append(v);
                    dg.insert(QStringLiteral("instances"), list);
                    if (!dg.contains(QStringLiteral("hidden")))
                        dg.insert(QStringLiteral("hidden"), g.value(QStringLiteral("hidden")).toBool(false));
                    dstGroups.insert(it.key(), dg);
                }
                dst.insert(QStringLiteral("formatVersion"), QStringLiteral("1"));
                dst.insert(QStringLiteral("groups"), dstGroups);
                if (!writeJson(dstFile, dst))
                    r.errors << QStringLiteral("could not write instgroups.json");
            }
        }
    }

    if (options.accounts && source.hasAccounts) {
        const QString target = QDir(destDataDir).filePath(QStringLiteral("accounts.json"));
        if (QFileInfo::exists(target)) {
            r.accountsSkipped = true;  // keep the accounts already signed in here
        } else {
            say(QStringLiteral("Copying accounts"));
            QDir().mkpath(destDataDir);
            if (QFile::copy(QDir(source.dataDir).filePath(QStringLiteral("accounts.json")), target))
                r.accountsCopied = true;
            else
                r.errors << QStringLiteral("could not copy accounts.json");
        }
    }

    if (options.icons && source.iconCount > 0) {
        const QString from = QDir(source.dataDir).filePath(QStringLiteral("icons"));
        const QString to = QDir(destDataDir).filePath(QStringLiteral("icons"));
        QDir().mkpath(to);
        for (const auto& name : QDir(from).entryList(QDir::Files)) {
            const QString target = QDir(to).filePath(name);
            if (QFileInfo::exists(target))
                continue;
            if (QFile::copy(QDir(from).filePath(name), target))
                ++r.iconsCopied;
        }
    }

    if (options.settings && !source.configFile.isEmpty()) {
        for (const auto& key : settingsWhitelist()) {
            const QString v = readIniValue(source.configFile, key);
            if (!v.isEmpty())
                r.settings.insert(key, v);
        }
    }
    return r;
}
