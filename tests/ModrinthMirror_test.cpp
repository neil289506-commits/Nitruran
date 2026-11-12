// SPDX-License-Identifier: GPL-3.0-only
#include <QTest>

#include <modplatform/ModrinthMirror.h>

struct FakeVersion {
    QString downloadUrl;
    QString hash;
    QString hashType;
};

// single quotes -> double quotes, keeps the test free of raw string literals
static QByteArray json(const char* text)
{
    return QString::fromUtf8(text).replace(QLatin1Char('\''), QLatin1Char('"')).toUtf8();
}

class ModrinthMirrorTest : public QObject {
    Q_OBJECT

   private slots:
    void parseAcceptsOnlyIdenticalFilesOnModrinthCdn()
    {
        const QByteArray resp = json(
            "{'AAAA1111': {'files':[{'hashes':{'sha1':'aaaa1111'},'url':'https://cdn.modrinth.com/data/p/versions/v/a.jar','filename':'a.jar'}]},"
            " 'bbbb2222': {'files':[{'hashes':{'sha1':'different'},'url':'https://cdn.modrinth.com/data/p/versions/v/b.jar'}]},"
            " 'cccc3333': {'files':[{'hashes':{'sha1':'cccc3333'},'url':'https://evil.example/c.jar'}]},"
            " 'dddd4444': {'files':[{'hashes':{'sha1':'other'},'url':'https://cdn.modrinth.com/x/o.jar'},"
            "                      {'hashes':{'sha1':'dddd4444'},'url':'https://cdn.modrinth.com/x/d.jar','filename':'d.jar'}]}}");
        const auto m = ModrinthMirror::parseVersionFiles(resp);
        QCOMPARE(m.size(), 2);
        QVERIFY(m.contains("aaaa1111"));
        QVERIFY(m.contains("dddd4444"));
        QVERIFY(!m.contains("bbbb2222"));  // hash differs -> not the same file
        QVERIFY(!m.contains("cccc3333"));  // not Modrinth's CDN
        QVERIFY(m["dddd4444"].url.endsWith("d.jar"));
        QVERIFY(ModrinthMirror::parseVersionFiles("garbage").isEmpty());
    }

    void fillOnlyBlockedSha1Versions()
    {
        const auto m = ModrinthMirror::parseVersionFiles(
            json("{'aaaa1111':{'files':[{'hashes':{'sha1':'aaaa1111'},'url':'https://cdn.modrinth.com/a.jar'}]}}"));
        QVector<FakeVersion> vs = { { "", "AAAA1111", "sha1" },
                                    { "", "bbbb2222", "sha1" },
                                    { "https://x/y.jar", "aaaa1111", "sha1" },
                                    { "", "aaaa1111", "md5" },
                                    { "", "", "sha1" } };
        const auto hashes = ModrinthMirror::blockedHashes(vs);
        QCOMPARE(hashes.size(), 2);
        QVERIFY(hashes.contains("aaaa1111") && hashes.contains("bbbb2222"));
        QCOMPARE(ModrinthMirror::fillBlocked(vs, m), 1);
        QVERIFY(vs[0].downloadUrl.endsWith("a.jar"));
        QCOMPARE(vs[2].downloadUrl, QString("https://x/y.jar"));  // already had a URL: untouched
        QVERIFY(vs[1].downloadUrl.isEmpty());
        QVERIFY(vs[3].downloadUrl.isEmpty());  // wrong hash type: untouched
    }
};

QTEST_GUILESS_MAIN(ModrinthMirrorTest)

#include "ModrinthMirror_test.moc"
