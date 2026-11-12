// SPDX-License-Identifier: GPL-3.0-only
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>
#include <QTest>

#include <ui/nitruran/SyncService.h>

static void touch(const QString& path, const QByteArray& content = "x")
{
    QDir().mkpath(QFileInfo(path).absolutePath());
    QFile f(path);
    QVERIFY2(f.open(QIODevice::WriteOnly), qPrintable(path));
    f.write(content);
}

static QByteArray slurp(const QString& path)
{
    QFile f(path);
    return f.open(QIODevice::ReadOnly) ? f.readAll() : QByteArray();
}

class NitruranSyncTest : public QObject {
    Q_OBJECT

    QTemporaryDir m_root;
    QString m_own;

    void build()
    {
        const QString p = m_root.filePath("PrismLauncher");
        touch(p + "/instances/Alpha/instance.cfg");
        touch(p + "/instances/Alpha/.minecraft/saves/w/level.dat", "lvl");
        touch(p + "/instances/Beta/instance.cfg");
        touch(p + "/instances/_LAUNCHER_TEMP/instance.cfg");
        touch(p + "/accounts.json", "{\"accounts\":[]}");
        touch(p + "/icons/a.png");
        touch(p + "/prismlauncher.cfg", "[General]\nMaxMemAlloc=4096\nJvmArgs=\"-Xss1m\"\nLanguage=zh_TW\nSecret=nope\n");
        touch(p + "/instances/instgroups.json",
              "{\"formatVersion\":\"1\",\"groups\":{\"Mods\":{\"hidden\":false,\"instances\":[\"Alpha\",\"Beta\"]}}}");

        const QString m = m_root.filePath("mlcmLauncher");
        const QString mi = m_root.filePath("elsewhere/inst");
        touch(mi + "/Gamma/instance.cfg");
        touch(m + "/mlcmlauncher.cfg", QString("[General]\nInstanceDir=%1\n").arg(mi).toUtf8());

        m_own = m_root.filePath("Nitruran");
        touch(m_own + "/instances/Beta/instance.cfg", "mine");
        touch(m_own + "/accounts.json", "{\"mine\":1}");
        touch(m_own + "/instances/instgroups.json",
              "{\"formatVersion\":\"1\",\"groups\":{\"Mods\":{\"hidden\":false,\"instances\":[\"Beta\"]}}}");
    }

   private slots:
    void detectAndSync()
    {
        QVERIFY(m_root.isValid());
        build();

        const auto sources = SyncService::detect(m_root.path(), m_own);
        QCOMPARE(sources.size(), 2);
        const SyncSource prism = sources[0].id == "PrismLauncher" ? sources[0] : sources[1];
        const SyncSource mlcm = sources[0].id == "mlcmLauncher" ? sources[0] : sources[1];
        QCOMPARE(prism.instances.size(), 2);  // temp folder ignored
        QVERIFY(prism.hasAccounts);
        QCOMPARE(prism.iconCount, 1);
        QCOMPARE(mlcm.instances, QStringList{ "Gamma" });  // absolute InstanceDir from its cfg

        const auto r = SyncService::sync(prism, m_own, m_own + "/instances", SyncOptions{});
        QCOMPARE(r.instancesCopied, 1);
        QCOMPARE(r.instancesSkipped, 1);
        QVERIFY(QFile::exists(m_own + "/instances/Alpha/.minecraft/saves/w/level.dat"));
        QCOMPARE(slurp(m_own + "/instances/Beta/instance.cfg"), QByteArray("mine"));  // never overwritten
        QVERIFY(r.accountsSkipped && !r.accountsCopied);
        QCOMPARE(slurp(m_own + "/accounts.json"), QByteArray("{\"mine\":1}"));
        QCOMPARE(r.iconsCopied, 1);
        QCOMPARE(r.settings.value("MaxMemAlloc"), QString("4096"));
        QCOMPARE(r.settings.value("JvmArgs"), QString("-Xss1m"));
        QCOMPARE(r.settings.value("Language"), QString("zh_TW"));
        QVERIFY(!r.settings.contains("Secret"));  // not whitelisted

        const auto groups = QJsonDocument::fromJson(slurp(m_own + "/instances/instgroups.json"))
                                .object()["groups"].toObject()["Mods"].toObject()["instances"].toArray();
        QCOMPARE(groups.size(), 2);
        QVERIFY(groups.contains("Alpha") && groups.contains("Beta"));

        const auto again = SyncService::sync(prism, m_own, m_own + "/instances", SyncOptions{});  // idempotent
        QCOMPARE(again.instancesCopied, 0);
        QCOMPARE(again.instancesSkipped, 2);
        QVERIFY(again.errors.isEmpty());

        const auto fresh = SyncService::sync(mlcm, m_root.filePath("fresh"), m_root.filePath("fresh/instances"), SyncOptions{});
        QCOMPARE(fresh.instancesCopied, 1);
    }
};

QTEST_GUILESS_MAIN(NitruranSyncTest)

#include "NitruranSync_test.moc"
