#!/usr/bin/env python3
"""Encrypts an API key so it can sit in a public repository without being greppable.

    pip install cryptography
    python tools/encrypt_secret.py

Prints two values for CMakeLists.txt:
    Launcher_CURSEFORGE_API_KEY_ENCRYPTED   base64(iv || ciphertext)   (AES-128-CTR, random key and iv)
    Launcher_CURSEFORGE_API_KEY_DECRYPT_KEY base64(key)

This only keeps automated scrapers away. The decryption key ships with the source and the binary, so anyone
who looks can recover the key. Use a key you can afford to lose.
"""
import base64
import getpass
import os

from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes


def main():
    secret = getpass.getpass("API key (input hidden): ").strip()
    if not secret:
        raise SystemExit("empty key")
    key, iv = os.urandom(16), os.urandom(16)
    ct = Cipher(algorithms.AES(key), modes.CTR(iv)).encryptor().update(secret.encode("utf-8"))
    payload = base64.b64encode(iv + ct).decode()
    key_b64 = base64.b64encode(key).decode()

    # self-check: decrypt what we just produced
    raw = base64.b64decode(payload)
    back = Cipher(algorithms.AES(base64.b64decode(key_b64)), modes.CTR(raw[:16])).decryptor().update(raw[16:])
    assert back.decode("utf-8") == secret, "round trip failed"

    print("\nLauncher_CURSEFORGE_API_KEY_ENCRYPTED   =", payload)
    print("Launcher_CURSEFORGE_API_KEY_DECRYPT_KEY =", key_b64)


if __name__ == "__main__":
    main()
