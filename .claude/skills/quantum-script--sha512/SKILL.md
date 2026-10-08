---
name: quantum-script--sha512
description: >-
  How to use the Quantum Script SHA512 extension (quantum-script--sha512),
  the SHA-512 hash functions loaded with Script.requireExtension("SHA512"):
  SHA512.hash(data) (128 lowercase hex characters), SHA512.hashToBuffer(data)
  (64 byte Buffer), SHA512.fileHash(filename) (hex, or undefined on error);
  how arguments are converted (bytes of toString(): Buffer first length bytes,
  numbers as text, null -> "null", missing argument -> "undefined", not
  empty); checksum files, release/<name>.sha512.json written by fabricare
  release, verifying downloads, cache keys, hashing binary data, comparing
  with sha512sum / Get-FileHash; what it is not for (passwords, HMAC);
  SHA512 is the hash embedded in fabricare (SHA256 / MD5 are not); the C++
  side (registerInternalExtension, initExecutive, Buffer registered first,
  quantum-script--sha512.static, XYO::Cryptography::SHA512 hash / hashToU8 /
  processU8 / processDone, Util::fileHashSHA512). Use when writing or
  reviewing Quantum Script or fabricare .js code that calls SHA512, C++ code
  that includes <XYO/QuantumScript.Extension/SHA512.hpp>, a fabricare.json
  depending on "quantum-script--sha512" or "quantum-script--sha512.static",
  or when working inside the quantum-script--sha512 repository.
---

# quantum-script--sha512

SHA-512 extension of Quantum Script (see the `quantum-script` skill for the
language and its differences from JavaScript, and the
`quantum-script--buffer` skill for the `Buffer` type; their rules apply).
Purpose: **checksums and content identifiers in scripts** — verify
downloads, write release checksum files, detect changed content, cache keys
— with the same digests as `sha512sum`. It is the hash built into
**fabricare** (release archives get their `SHA512.fileHash` recorded in
`release/<project>.v<version>.sha512.json`).

Full documentation: `docs/` in the quantum-script--sha512 repository
(`X:\Storage\XYO\Gitea\CPP\quantum-script--sha512\docs` on this machine):
README, getting-started, **script-api** (argument conversion, exact
results, errors), **recipes** (verify / write checksum files, fabricare
JSON format, folders, binary data, cache keys, what not to use it for),
cpp-api, reference. Read the matching page when you need more than this
summary. The whole implementation is
`source/XYO/QuantumScript.Extension/SHA512/Library.cpp` (~75 lines) on top
of `xyo-cryptography`.

## Script API

```javascript
Script.requireExtension("SHA512");          // also loads Buffer

SHA512.hash("abc");                         // "ddaf35a1...a54ca49f", 128 lowercase hex characters
SHA512.hash("");                            // "cf83e1357eefb8bd...f927da3e"
SHA512.hash(buffer);                        // first buffer.length bytes, zero bytes included
var d = SHA512.hashToBuffer("abc");         // Buffer, size == length == 64; d.toHex() == SHA512.hash("abc")
var h = SHA512.fileHash("archive.zip");     // hex String, or undefined
if (Script.isUndefined(h)) { throw(new Error("cannot hash archive.zip")); };
```

## Hard rules

1. **Only three functions**: `hash`, `hashToBuffer`, `fileHash`. No
   `update` / `digest` streaming, no HMAC, no `SHA512(...)` call, no
   `new SHA512`. `SHA512` is a plain object (`typeof(SHA512) == "Object"`).
   For data in pieces: join first (`parts.join("")`), or write a file and
   use `fileHash`.
2. **The argument is hashed as `toString()` bytes.** `SHA512.hash(123)` ==
   `SHA512.hash("123")`; `true` -> `"true"`; `[1,2]` -> `"1,2"`; `{}` ->
   `"Object"`; `null` -> `"null"`; **`SHA512.hash()` / `undefined` hash the
   text `"undefined"`** (`16a332e8...`), not the empty input (`cf83e135...`).
   Guard optional values; use `SHA512.hash("")` for the empty digest.
3. **Bytes, no transcoding.** Strings are hashed as stored (UTF-8 from
   scripts and `Shell.fileGetContents`), so `SHA512.hash("ă")` matches
   `printf 'ă' | sha512sum`. Buffers hash their first `length` bytes, not
   `size`.
4. **`fileHash` returns `undefined` on any failure** (missing file,
   directory, empty name, no permission, short read) and never throws.
   Always test with `Script.isUndefined(h)` before using it — `"" + undefined`
   is the text `"undefined"`, and `json[name] = undefined` silently drops the
   entry. Paths are relative to the process working directory, not the
   script.
5. **Output is lowercase hex, 128 characters.** Normalize external digests
   before `==`: `expected.trim().toLowerCaseASCII()`. `.sha512` files hold
   `<hex> *<name>` or `<hex>  <name>`: take `substring(0, 128)` (not 64 —
   that is SHA-256 length).
6. **`fileHash` for files, not `hash(Shell.fileGetContents(...))`**: it
   streams 32 KB chunks and detects incomplete reads. Read the file into a
   buffer only when the bytes are needed anyway
   (`SHA512.hash(Shell.fileGetContentsBuffer(name))` gives the same digest).
7. **Raw bytes**: `hashToBuffer` returns a new 64 byte `Buffer` each call;
   `Buffer.fromHex(SHA512.fileHash(name))` for a file's raw digest;
   `Base64.encode(d)` for base64 (88 characters, Subresource Integrity
   `sha512-...`); `SHA512.hash(d)` is double SHA-512.
8. **Truncation is not another algorithm**: `hash(x).substring(0, 64)` is a
   valid shorter key but is neither SHA-256 nor SHA-512/256.
9. **Concatenation is ambiguous**: `hash("ab" + "c") == hash("a" + "bc")`.
   For keys from several fields add separators or length prefixes
   (`s.length + ":" + s + ";"`).
10. **Not for passwords, MACs or secrets**: plain / salted SHA-512 is too fast
    for passwords (no Quantum Script extension provides Argon2 / scrypt /
    bcrypt / PBKDF2); `hash(secret + message)` is not a MAC (length
    extension, no HMAC here); string `==` is not constant time.
11. **In fabricare scripts use SHA512**: fabricare is static, embeds
    `SHA512` (its `Library.js` already requires it) and cannot load DLLs;
    `SHA256` / `MD5` are not available there.
12. **One engine per thread**: each thread requires `SHA512` itself; the
    functions keep no state.

## Fabricare release checksums

```javascript
// what fabricare release / gitea-release do for every archive
json[Shell.getFileName(file)] = SHA512.fileHash(file);
Shell.filePutContents(jsonFilename, JSON.encodeWithIndentation(json));
// verify: SHA512.fileHash(name) == JSON.decode(Shell.fileGetContents(jsonFilename))[name]
```

## Related

- `SHA256`: same three functions, 32 byte digest. `MD5`: `hash` /
  `hashToBuffer` only, 16 bytes, not for security.
- `OpenSSL.sha512(bufferIn, st, ln, hashOut)`: SHA-512 of a buffer range
  through OpenSSL.

## C++

```cpp
#include <XYO/QuantumScript.Extension/Buffer.hpp>
#include <XYO/QuantumScript.Extension/SHA512.hpp>
using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {                 // host init callback
	Extension::Buffer::registerInternalExtension(executive);   // required: SHA512 requires Buffer while initializing
	Extension::SHA512::registerInternalExtension(executive);   // scripts still call requireExtension("SHA512")
};
```

Hashing in C++ without a script (`#include <XYO/Cryptography.hpp>`,
dependency `xyo-cryptography`):

```cpp
String hex = XYO::Cryptography::SHA512::hash(str);                // lowercase hex, 128 characters
uint8_t digest[64]; XYO::Cryptography::SHA512::hashToU8(str, digest);
String fileHex; bool ok = XYO::Cryptography::Util::fileHashSHA512("f.bin", fileHex);
XYO::Cryptography::SHA512 sha;                                    // constructor = processInit()
sha.processU8(data, size); sha.processDone();                     // processDone once, then:
sha.getHashHex(); sha.toU8(digest); sha.processInit();            // processInit before reuse
```

- `fabricare.json` dependency: `"quantum-script--sha512"` (brings
  `quantum-script`, `quantum-script--console`, `quantum-script--buffer`,
  `xyo-cryptography`; `dll-or-lib`). For self-contained executables:
  `"quantum-script--sha512.static"` with the other `.static` dependencies
  and `"crt": "static"`; it defines
  `XYO_QUANTUMSCRIPT_EXTENSION_SHA512_LIBRARY` for consumers (empty export
  macro, no `quantumScriptExtension` entry point), so register it (and
  `Buffer`) as internal. `..._INTERNAL`: building the DLL.

## Working in this repository

- Build: `fabricare make`, `fabricare test` (runs `test/test.01`, a host
  registering Console, Buffer and SHA512 as internal and running
  `test/test.01.js`, which checks `SHA512.hash` against known digests; run
  `make` first), `fabricare install` (see the `fabricare` skill).
  `quantum-script`, `quantum-script--console`, `quantum-script--buffer` and
  `xyo-cryptography` must be installed first.
- Quick check with the installed DLL:
  `quantum-script script.js` where the script requires `SHA512`; compare
  with `printf 'abc' | sha512sum`.
- Native functions live in `SHA512/Library.cpp` as
  `static TPointer<Variable> name(VariableFunction *, Variable *this_, VariableArray *arguments)`
  and are registered in `initExecutive` with
  `executive->setFunction2("SHA512.name(args)", name)`.
- Changing the API: update `README.md`, `docs/script-api.md`,
  `docs/reference.md`, `docs/recipes.md` when relevant, and this skill. Keep
  it in step with `quantum-script--sha256` (same API shape). fabricare
  embeds this extension: an API change affects fabricare scripts too.
- Code style: tabs (width 8), `.clang-format`, CRLF, statements and blocks
  end with `};`, camelCase. SPDX header: MIT for `source/` and `docs/`,
  Unlicense for `test/` and `.claude/` (see `.reuse/dep5`).
