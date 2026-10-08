# Quantum Script Extension SHA512

Quantum Script extension
- SHA-512 hashing for scripts: `SHA512.hash` returns the digest as 128
lowercase hex characters (the same text `sha512sum` prints).
- The raw 64 byte digest as a `Buffer` (`SHA512.hashToBuffer`).
- Checksums of files of any size, read in chunks (`SHA512.fileHash`),
`undefined` when the file cannot be read.
- Strings and buffers are hashed byte for byte, zero bytes included.
- Embedded in fabricare, which uses it for release checksums.

```javascript
Script.requireExtension("SHA512");

SHA512;
SHA512.hash(str);
SHA512.hashToBuffer(str);
SHA512.fileHash(filename);
```

Built on `quantum-script`, `quantum-script--buffer` and `xyo-cryptography`,
part of the XYO C++ SDK.

## Documentation

- [Overview](docs/README.md) - purpose and design
- [Getting started](docs/getting-started.md) - build, load from a script or fabricare, register in a C++ host, static builds
- [Script API](docs/script-api.md) - every function: argument conversion, exact results, errors
- [Recipes](docs/recipes.md) - verify and write checksum files, fabricare release JSON, hash folders and binary data, cache keys, what SHA-512 is not for
- [C++ API](docs/cpp-api.md) - `registerInternalExtension`, `initExecutive`, hashing from C++ with `xyo-cryptography`
- [API reference](docs/reference.md)

A Claude Code skill for this extension is in
[.claude/skills/quantum-script--sha512](.claude/skills/quantum-script--sha512/SKILL.md).

## License

Copyright (c) 2016-2026 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
