// SPDX-License-Identifier: GPL-3.0-only
#include "ModrinthMirror.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace ModrinthMirror {

QHash<QString, Mirror> parseVersionFiles(const QByteArray& response)
{
    QHash<QString, Mirror> out;
    const auto doc = QJsonDocument::fromJson(response);
    if (!doc.isObject())
        return out;
    const auto root = doc.object();
    for (auto it = root.begin(); it != root.end(); ++it) {
        const QString key = it.key().toLower();
        const auto files = it.value().toObject().value(QStringLiteral("files")).toArray();
        for (const auto& fv : files) {
            const auto file = fv.toObject();
            const QString fileSha1 = file.value(QStringLiteral("hashes")).toObject().value(QStringLiteral("sha1")).toString().toLower();
            const QString url = file.value(QStringLiteral("url")).toString();
            if (fileSha1 != key || !url.startsWith(QLatin1String("https://cdn.modrinth.com/")))
                continue;  // not the identical file, or not hosted by Modrinth
            out.insert(key, { key, url, file.value(QStringLiteral("filename")).toString() });
            break;
        }
    }
    return out;
}

}  // namespace ModrinthMirror
