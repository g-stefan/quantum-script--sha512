# API reference

## Script

| Symbol | Returns | Description |
|--------|---------|-------------|
| `Script.requireExtension("SHA512")` | | load the extension (also loads `Buffer`) |
| `SHA512` | `Object` | holder of the functions below, not a constructor |
| `SHA512.hash(data)` | `String` | SHA-512 of `data.toString()` bytes, 128 lowercase hex characters |
| `SHA512.hashToBuffer(data)` | `Buffer` | SHA-512 of `data.toString()` bytes, 64 raw bytes (`size` = `length` = 64) |
| `SHA512.fileHash(filename)` | `String` or `undefined` | SHA-512 of the file content, 128 lowercase hex characters; `undefined` if the file cannot be read completely |

Argument conversion for `hash` / `hashToBuffer`:

| Value | Hashed as |
|-------|-----------|
| `String` | its bytes |
| `Buffer` | its first `length` bytes |
| `Number`, `Boolean`, `Array`, `Object` | their text (`123` -> `"123"`, `true` -> `"true"`, `[1,2]` -> `"1,2"`, `{}` -> `"Object"`) |
| `null` | `"null"` |
| `undefined` / missing | `"undefined"` |

Known values:

| Input | `SHA512.hash(input)` |
|-------|----------------------|
| `""` | `cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e` |
| `"abc"` | `ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f` |
| `"The quick brown fox jumps over the lazy dog"` | `07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6` |
| missing argument (`"undefined"`) | `16a332e891e86030aa9d08ab032fe026c4d4857b64902c386f3ede705373ecf9206f58d712a91a07a63dcbd14f133ab48571bfeb88927995224b299916af8fa5` |

## C++ — `XYO::QuantumScript::Extension::SHA512`

| Symbol | Header | Description |
|--------|--------|-------------|
| `void registerInternalExtension(Executive *executive)` | `SHA512/Library.hpp` | register `SHA512` as an internal extension |
| `void initExecutive(Executive *executive, void *extensionId)` | `SHA512/Library.hpp` | extension initialization: metadata, `Buffer`, `SHA512` object and functions |
| `extern "C" void quantumScriptExtension(Executive *, void *)` | `SHA512/Library.cpp` | DLL entry point, forwards to `initExecutive` |
| `const char *Version::version()` / `build()` / `versionWithBuild()` / `datetime()` | `SHA512/Version.hpp` | version info from `version.json` |
| `std::string License::license()` / `shortLicense()` | `SHA512/License.hpp` | MIT license text |
| `const char *Copyright::copyright()` / `publisher()` / `company()` / `contact()` | `SHA512/Copyright.hpp` | copyright info |
| `XYO_QUANTUMSCRIPT_EXTENSION_SHA512_EXPORT` | `SHA512/Dependency.hpp` | import / export macro |
| `XYO_QUANTUMSCRIPT_EXTENSION_SHA512_INTERNAL` | | define when building the DLL |
| `XYO_QUANTUMSCRIPT_EXTENSION_SHA512_LIBRARY` | | define for static use: empty export macro, no DLL entry point |

Umbrella header: `<XYO/QuantumScript.Extension/SHA512.hpp>`.
Whole extension in one translation unit: `SHA512.Amalgam.cpp`.

## C++ — `XYO::Cryptography` (used by the extension)

| Symbol | Description |
|--------|-------------|
| `static String SHA512::hash(const String &toHash)` | one shot, lowercase hex |
| `static void SHA512::hashToU8(const String &toHash, uint8_t *buffer)` | one shot, 64 raw bytes into `buffer` |
| `SHA512()` / `processInit()` | start (or restart) a computation |
| `processU8(const uint8_t *toHash, size_t length)` | add bytes |
| `processDone()` | finalize, call once |
| `String getHashHex()` / `void toU8(uint8_t *buffer)` | read the result after `processDone()` |
| `copy(const SHA512 &in)` | duplicate a state |
| `bool Util::fileHashSHA512(const char *fileName, String &hash)` | hex digest of a file, `false` on any error |

## fabricare.json

| Project | Make | Depends on |
|---------|------|------------|
| `quantum-script--sha512` | `dll-or-lib` (DLL on dynamic platforms, static library on static ones) | `quantum-script`, `quantum-script--console`, `quantum-script--buffer`, `xyo-cryptography` |
| `quantum-script--sha512.static` | `lib`, static CRT, exports `XYO_QUANTUMSCRIPT_EXTENSION_SHA512_LIBRARY` | the `.static` variants of the same |
| `test.01` | `exe`, category `test` | `quantum-script--sha512` |

Known users: `fabricare` (`quantum-script--sha512.static`, release
checksums), `quantum-script--magnet`.

## Related extensions

| Extension | Functions | Digest |
|-----------|-----------|--------|
| `SHA256` | `hash`, `hashToBuffer`, `fileHash` | 32 bytes / 64 hex characters (not embedded in fabricare) |
| `MD5` | `hash`, `hashToBuffer` | 16 bytes / 32 hex characters (not for security) |
| `OpenSSL` | `OpenSSL.sha512(bufferIn, st, ln, hashOut)` | SHA-512 of a buffer range through OpenSSL |
| `Buffer` | the type returned by `hashToBuffer` | |
