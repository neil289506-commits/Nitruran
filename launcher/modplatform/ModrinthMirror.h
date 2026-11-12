// SPDX-License-Identifier: GPL-3.0-only
// Nitruran: CurseForge files whose author disabled third-party distribution have no download URL.
// If the *byte-identical* file (same SHA-1) is published on Modrinth, we may download that copy instead.
// This only ever uses Modrinth's public API and CDN; it never touches CurseForge's restriction.
#pragma once

#include <QByteArray>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QVector>

namespace ModrinthMirror {

struct Mirror {
    QString sha1;
    QString url;       // https://cdn.modrinth.com/...
    QString fileName;
};

/// Parses the response of POST /v2/version_files (algorithm=sha1): { "<sha1>": { files: [...] }, ... }.
/// Only files whose own sha1 equals the requested key and whose URL is on Modrinth's CDN are accepted.
QHash<QString, Mirror> parseVersionFiles(const QByteArray& response);

/// Fills downloadUrl for versions that have none, hashType == "sha1" and a mirror. Works on any version type
/// with the members downloadUrl / hash / hashType. Returns how many versions were filled.
template <typename Version>
int fillBlocked(QVector<Version>& versions, const QHash<QString, Mirror>& mirrors)
{
    int filled = 0;
    for (auto& v : versions) {
        if (!v.downloadUrl.isEmpty() || v.hashType != QLatin1String("sha1") || v.hash.isEmpty())
            continue;
        const auto it = mirrors.constFind(v.hash.toLower());
        if (it == mirrors.constEnd())
            continue;
        v.downloadUrl = it->url;
        ++filled;
    }
    return filled;
}

/// sha1 hashes (lower-case) of versions that are blocked and could be looked up.
template <typename Version>
QStringList blockedHashes(const QVector<Version>& versions)
{
    QStringList out;
    for (const auto& v : versions)
        if (v.downloadUrl.isEmpty() && v.hashType == QLatin1String("sha1") && !v.hash.isEmpty() && !out.contains(v.hash.toLower()))
            out << v.hash.toLower();
    return out;
}

}  // namespace ModrinthMirror
