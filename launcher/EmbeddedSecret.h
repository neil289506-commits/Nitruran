// SPDX-License-Identifier: GPL-3.0-only
// Nitruran: lets a public repository carry an API key as AES-encrypted text instead of plain text.
// This only keeps automated scrapers away from the string; anyone who reads the source or the binary can
// still recover the key, because the decryption key ships with it. Do not use it for real secrets.
#pragma once

#include <QByteArray>
#include <QString>

namespace EmbeddedSecret {

/// AES-128 in CTR mode (the same operation encrypts and decrypts). key: 16 bytes, iv: 16-byte initial counter block.
/// Returns an empty array when key or iv have the wrong size.
QByteArray aes128Ctr(const QByteArray& key, const QByteArray& iv, const QByteArray& data);

/// payloadBase64 = base64(iv[16] || ciphertext), keyBase64 = base64(16-byte key).
/// Returns the plain text, or an empty string if anything is missing or malformed.
QString decrypt(const QString& payloadBase64, const QString& keyBase64);

}  // namespace EmbeddedSecret
