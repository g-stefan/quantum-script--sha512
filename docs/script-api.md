# Script API

```javascript
Script.requireExtension("SHA512");

SHA512;                      // plain object holding the functions
SHA512.hash(data);           // String, 128 lowercase hex characters
SHA512.hashToBuffer(data);   // Buffer, 64 bytes
SHA512.fileHash(filename);   // String, 128 lowercase hex characters, or undefined
```

`SHA512` is a global variable created by the extension
(`var SHA512={};`) with three native functions. It is not a constructor and
has no state: every call starts a new SHA-512 computation and finishes it
before returning. There is no incremental (`update` / `digest`) interface; to
hash data that arrives in pieces, collect it first, or write it to a file and
use `fileHash`.

## Loading

```javascript
Script.requireExtension("SHA512");
```

- Loads `quantum-script--sha512` (external DLL, or an internal extension
  registered by the host, as in fabricare) and runs its `initExecutive`.
- `initExecutive` runs `Script.requireExtension("Buffer")` first, so `Buffer`
  is available afterwards, then defines `SHA512` and its functions.
- Requiring it again does nothing.
- The extension is public, versioned and listed by `Script.getExtensionList()`
  with the name `SHA512`.

## How the argument is converted

`hash` and `hashToBuffer` take any value and hash the bytes of
`value.toString()` (the C++ `Variable::toString()`):

| Argument | Bytes hashed | Example |
|----------|--------------|---------|
| `String` | its bytes (UTF-8), zero bytes included | `SHA512.hash("ă")` hashes the 2 bytes `c4 83` |
| `Buffer` | its first `length` bytes, unchanged | `SHA512.hash(Buffer.fromHex("00ff00"))` hashes 3 bytes |
| `Number` | its text | `SHA512.hash(123) == SHA512.hash("123")`, `0.1` hashes `"0.1"` |
| `Boolean` | `"true"` / `"false"` | |
| `null` | `"null"` | **not** the empty input |
| `undefined`, missing argument | `"undefined"` | `SHA512.hash()` is `16a332e8...`, **not** `cf83e135...` |
| `Array` | its text | `[1,2]` hashes `"1,2"` |
| `Object` | its text | `{}` hashes `"Object"` |

There is no character set conversion: a string is hashed exactly as stored.
Scripts, `Shell.fileGetContents` and most extensions produce UTF-8, which
matches what other tools hash for the same text. The digest of the empty
input is obtained with `SHA512.hash("")` or an empty buffer.

Extra arguments are ignored.

## SHA512.hash(data)

Returns the SHA-512 digest of `data` (converted as above) as a `String` of
**128 lowercase hexadecimal characters**, the same text as `sha512sum`,
`openssl dgst -sha512`, `Get-FileHash -Algorithm SHA512` (after lowercasing)
or Python's `hashlib.sha512(...).hexdigest()`.

```javascript
SHA512.hash("");
// "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e"
SHA512.hash("abc");
// "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f"
SHA512.hash("The quick brown fox jumps over the lazy dog");
// "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6"
```

Never throws for a valid call; any value is accepted.

## SHA512.hashToBuffer(data)

Returns the digest of `data` (converted as above) as a new `Buffer` with
`size` 64 and `length` 64: the raw digest bytes, most significant byte of the
first 64 bit word first (the standard SHA-512 byte order).

```javascript
var d = SHA512.hashToBuffer("abc");
d.length;               // 64
d.getU8(0);             // 221 (0xdd)
d.toHex();              // same as SHA512.hash("abc")
SHA512.hash(d);         // SHA-512 of the 64 digest bytes (double SHA-512)
```

Use it when the bytes themselves are needed: as key material, inside a binary
file or packet (`file.writeFromBuffer(d)`), to feed another hash, or to
encode the digest differently (`Base64.encode(d)`).

Each call returns a new buffer; changing it does not affect anything else.

## SHA512.fileHash(filename)

Returns the SHA-512 digest of the file `filename` (converted to a string) as
128 lowercase hexadecimal characters, or `undefined` if the file cannot be
hashed.

```javascript
var h = SHA512.fileHash("release/archive.zip");
if (Script.isUndefined(h)) {
	throw(new Error("cannot hash release/archive.zip"));
};
```

- The path is relative to the current working directory of the process, not
  to the script. Use an absolute path, or build one from
  `Script.getIncludedFile()` / `Shell.getFilePath(...)`, when the script can
  be started from elsewhere.
- The file is opened read only and read in 32 KB chunks, so files of any size
  can be hashed without loading them into memory.
- After reading, the number of bytes hashed is compared with the file size
  measured before reading; if they differ (read error, file truncated while
  being read) the result is `undefined` rather than a wrong digest.
- `undefined` is also returned when the file does not exist, cannot be
  opened (permissions, locked), or the name is a directory or empty.
- It never throws. Always check the result: `undefined` concatenated into a
  string becomes the text `"undefined"`, and stored in JSON it drops the key.
- An empty file gives the digest of the empty input, `cf83e135...927da3e`.

`fileHash` returns hex only. For the raw bytes use
`Buffer.fromHex(SHA512.fileHash(name))`, or for small files
`SHA512.hashToBuffer(Shell.fileGetContentsBuffer(name))`.

## Comparing digests

Digests from this extension are always lowercase. Text from elsewhere (a
`.sha512` file, a web page, `Get-FileHash` output) may be uppercase or carry
spaces and a file name; normalize before comparing:

```javascript
var expected = "DDAF35A193617ABACC417349AE20413112E6FA4E89A97EA20A9EEEE64B55D39A2192992A274FC1A836BA3C23A3FEEBBD454D4423643CE80E2A9AC94FA54CA49F";
SHA512.hash("abc") == expected.trim().toLowerCaseASCII();   // true
```

`==` on strings compares the whole text and stops at the first difference.
That is fine for integrity checks (checksums, cache keys); it is not a
constant time comparison for secrets.

## Errors

| Situation | Result |
|-----------|--------|
| `SHA512` used before `Script.requireExtension("SHA512")` | the usual undefined variable error |
| Extension library not found | `Script.requireExtension` throws `Unable to open "SHA512"` |
| `Buffer` extension not available (host registered `SHA512` but not `Buffer`, no Buffer DLL) | loading `SHA512` fails: its initialization requires `Buffer` |
| Any value passed to `hash` / `hashToBuffer` | hashed as its text, no error |
| File missing, unreadable, directory, short read | `fileHash` returns `undefined` |
