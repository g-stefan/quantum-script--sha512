# Quantum Script Extension SHA512 — Documentation

`quantum-script--sha512` is the **SHA-512 hash extension of Quantum Script**.
Loaded with `Script.requireExtension("SHA512")`, it adds a `SHA512` object to
scripts with three functions: hash a string or a buffer to hexadecimal text,
hash it to a 64 byte `Buffer`, and hash a whole file.

The hashing itself is `XYO::Cryptography::SHA512` from `xyo-cryptography`
(FIPS 180-4 SHA-512); this extension is the thin script binding on top of it.

- **`SHA512.hash(data)`** — the digest as 128 lowercase hexadecimal
  characters, the same text `sha512sum` prints.
- **`SHA512.hashToBuffer(data)`** — the raw 64 byte digest as a `Buffer`
  (`quantum-script--buffer`), for binary protocols, key material or further
  hashing.
- **`SHA512.fileHash(filename)`** — the hex digest of a file, read in 32 KB
  chunks (any size, no memory spike); `undefined` if the file cannot be read
  completely.

It is also the hash built into **fabricare**: the release scripts record the
`SHA512.fileHash` of every archive in `release/<name>.sha512.json`.

```
scripts: quantum-script .js, fabricare build scripts, tools and hosts that embed the engine
quantum-script--sha512    <-- this extension: SHA512.hash / hashToBuffer / fileHash
quantum-script--buffer    (the Buffer type returned by hashToBuffer)
quantum-script            (Executive, Variable, Context)
xyo-cryptography          (XYO::Cryptography::SHA512, Util::fileHashSHA512)
xyo-system, xyo-encoding, xyo-multithreading, xyo-data-structures, xyo-managed-memory, xyo-platform
```

## Why it exists

| Need | What `SHA512` gives |
|------|---------------------|
| Checksum a download or a release archive | `SHA512.fileHash("archive.zip")`, compare with the published `.sha512` / `.sha512.json` |
| Detect that content changed (cache keys, build stamps) | `SHA512.hash(text)`, a fixed 128 character key for any input |
| Identify data by content (deduplication, content addressing) | same input, same digest, on every platform |
| Raw digest bytes for another algorithm or a binary format | `SHA512.hashToBuffer(data)`, 64 bytes |
| Hash binary data, not only text | pass a `Buffer`: its bytes are hashed unchanged, zero bytes included |
| A hash inside fabricare build scripts | `SHA512` is embedded in fabricare; `SHA256` and `MD5` are not |

## Concepts at a glance

| Need | Use | Notes |
|------|-----|-------|
| Load the extension | `Script.requireExtension("SHA512");` | also loads `Buffer` |
| Hex digest of a string | `SHA512.hash("abc")` | `"ddaf35a1...a54ca49f"`, 128 lowercase hex characters |
| Hex digest of a buffer | `SHA512.hash(buffer)` | the first `length` bytes of the buffer |
| Raw digest | `SHA512.hashToBuffer("abc")` | `Buffer`, `size` = `length` = 64 |
| Raw digest as hex | `SHA512.hashToBuffer(x).toHex()` | equal to `SHA512.hash(x)` |
| Hash of a file | `SHA512.fileHash("file.bin")` | hex string, or `undefined` on any error |
| Compare digests | `a == b` | both lowercase; lowercase external text first if needed |
| Other algorithms | `SHA256`, `MD5` extensions | `SHA256`: same three functions, 32 byte digest; `MD5`: `hash` / `hashToBuffer` only, 16 bytes |

Arguments are converted to a string first, so numbers, booleans and objects
are hashed as their text: `SHA512.hash(123)` is the hash of `"123"`, and a
missing argument is the hash of the text `"undefined"`, **not** of the empty
string. See [Script API](script-api.md).

## Contents

| Document | What it covers |
|----------|----------------|
| [Getting started](getting-started.md) | Build and install, load the extension from a script or a fabricare script, register it in a C++ host, static builds |
| [Script API](script-api.md) | Every function: arguments, how values are converted, exact results, errors |
| [Recipes](recipes.md) | Verify a checksum file, write release checksums (fabricare style), hash many files, hash binary data, cache keys, what SHA-512 is not for |
| [C++ API](cpp-api.md) | `registerInternalExtension`, `initExecutive`, version info, hashing from C++ with `xyo-cryptography` |
| [API reference](reference.md) | Every script and C++ symbol on one page |

Quantum Script itself (the language, `Script.requireExtension`, embedding,
writing extensions) is documented in the `quantum-script` repository,
`docs/`; the `Buffer` type in the `quantum-script--buffer` repository,
`docs/`.

## Source map

```
source/XYO/QuantumScript.Extension/SHA512.hpp            umbrella header, include this from C++
source/XYO/QuantumScript.Extension/SHA512.Amalgam.cpp    the whole extension in one translation unit
source/XYO/QuantumScript.Extension/SHA512/
    Dependency.hpp                                       <XYO/QuantumScript.hpp>, export macro
    Library[.hpp/.cpp]                                   initExecutive, registerInternalExtension,
                                                         hash / hashToBuffer / fileHash
    Copyright / License / Version                        library metadata
test/test.01.cpp                                         C++ host registering Console, Buffer and SHA512 as internal
test/test.01.js                                          SHA512.hash against known digests
```

## AI assistant skill

A Claude Code skill describing how to use this extension lives in
[`.claude/skills/quantum-script--sha512/`](../.claude/skills/quantum-script--sha512/SKILL.md).
It is picked up automatically inside this repository; copy the folder to
`~/.claude/skills/` to have it available in the projects that use `SHA512`
(fabricare scripts, Quantum Script tools, hosts, other extensions).
