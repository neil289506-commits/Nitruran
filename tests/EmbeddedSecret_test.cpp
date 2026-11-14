// SPDX-License-Identifier: GPL-3.0-only
#include <QTest>

#include <EmbeddedSecret.h>

class EmbeddedSecretTest : public QObject {
    Q_OBJECT

   private slots:
    void fips197Vector()
    {
        // FIPS-197 appendix C.1: CTR over one zero block with the plaintext as counter block yields AES(plaintext)
        const QByteArray key = QByteArray::fromHex("000102030405060708090a0b0c0d0e0f");
        const QByteArray block = QByteArray::fromHex("00112233445566778899aabbccddeeff");
        QCOMPARE(EmbeddedSecret::aes128Ctr(key, block, QByteArray(16, '\0')).toHex(), QByteArray("69c4e0d86a7b0430d8cdb78070b4c55a"));
    }

    void matchesReferenceImplementation()
    {
        // expected bytes produced with Python's cryptography library (AES-CTR)
        const QByteArray key = QByteArray::fromHex("000102030405060708090a0b0c0d0e0f");
        const QByteArray iv = QByteArray::fromHex("f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");
        QByteArray plain(100, '\0');
        for (int i = 0; i < plain.size(); ++i)
            plain[i] = static_cast<char>(i);
        QCOMPARE(EmbeddedSecret::aes128Ctr(key, iv, plain).toHex(), QByteArray("66a6c5eb3057374f9f58d40c3f1ba3a2a290c513a38b2ababcb469a0728101f5f250b075587ecdbad3a8a17263bf7b5e40e95469088a6e706f543923735d09a52b40e06b69d31525cccb9359b3b3cf72b96573a331816f09f0d47f251c1266dc84673856"));
        QCOMPARE(EmbeddedSecret::aes128Ctr(key, iv, EmbeddedSecret::aes128Ctr(key, iv, plain)), plain);  // symmetric
    }

    void decryptsBase64Payload()
    {
        QCOMPARE(EmbeddedSecret::decrypt("8PHy8/T19vf4+fr7/P3+/zLilLwZE2EBuhqbXh4mnJ+BteI2gKYFzMbOF94I+3eDuBr+OxI0mw==", "AAECAwQFBgcICQoLDA0ODw=="), QString("TEST-API-KEY-0123456789abcdefghijklmnop"));
    }

    void badInputGivesEmptyString()
    {
        QVERIFY(EmbeddedSecret::decrypt("", "AAECAwQFBgcICQoLDA0ODw==").isEmpty());
        QVERIFY(EmbeddedSecret::decrypt("8PHy8/T19vf4+fr7/P3+/zLilLwZE2EBuhqbXh4mnJ+BteI2gKYFzMbOF94I+3eDuBr+OxI0mw==", "").isEmpty());
        QVERIFY(EmbeddedSecret::decrypt("8PHy8/T19vf4+fr7/P3+/zLilLwZE2EBuhqbXh4mnJ+BteI2gKYFzMbOF94I+3eDuBr+OxI0mw==", "AAAA").isEmpty());   // key too short
        QVERIFY(EmbeddedSecret::decrypt("AAAA", "AAECAwQFBgcICQoLDA0ODw==").isEmpty());      // payload shorter than an IV
        QVERIFY(EmbeddedSecret::aes128Ctr(QByteArray(15, 'k'), QByteArray(16, 'i'), "x").isEmpty());
    }
};

QTEST_GUILESS_MAIN(EmbeddedSecretTest)

#include "EmbeddedSecret_test.moc"
