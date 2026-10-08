# Recipes

All examples assume:

```javascript
Script.requireExtension("Console");
Script.requireExtension("SHA512");
```

## Print the checksum of a file

```javascript
var name = "archive.zip";
var h = SHA512.fileHash(name);
if (Script.isUndefined(h)) {
	Console.writeLn("error: cannot read " + name);
	Script.setExitCode(1);
} else {
	Console.writeLn(h + " *" + name);   // the sha512sum binary mode line format
};
```

## Verify a file against a published checksum

A `.sha512` file usually holds `<128 hex chars> *<file name>` or
`<128 hex chars>  <file name>`, sometimes in uppercase:

```javascript
Script.requireExtension("Shell");

function verifySHA512(fileName, checksumFile) {
	var text = Shell.fileGetContents(checksumFile);
	if (!text) {
		return false;
	};
	var expected = text.trim().substring(0, 128).toLowerCaseASCII();
	var actual = SHA512.fileHash(fileName);
	if (Script.isUndefined(actual)) {
		return false;
	};
	return actual == expected;
};

if (!verifySHA512("archive.zip", "archive.zip.sha512")) {
	throw(new Error("checksum mismatch: archive.zip"));
};
```

## Release checksums as JSON (the fabricare format)

fabricare's `release` action keeps one JSON file per version,
`release/<project>.v<version>.sha512.json`, mapping each archive name to its
digest, and adds an entry every time it creates an archive:

```javascript
Script.requireExtension("Shell");
Script.requireExtension("JSON");

function addReleaseHash(jsonFilename, file) {
	var json = {};
	var text = Shell.fileGetContents(jsonFilename);
	if (text) {
		json = JSON.decode(text);
		if (Script.isNil(json)) {
			json = {};
		};
	};
	var h = SHA512.fileHash(file);
	if (Script.isUndefined(h)) {
		throw(new Error("cannot hash " + file));
	};
	json[Shell.getFileName(file)] = h;
	Shell.filePutContents(jsonFilename, JSON.encodeWithIndentation(json));
};

addReleaseHash("release/my-app.v1.0.0.sha512.json", "release/my-app.v1.0.0.win64.bin.zip");
```

Result:

```json
{
	"my-app.v1.0.0.win64.bin.zip": "0c469c672f18b079..."
}
```

Verifying a downloaded archive against that file:

```javascript
var json = JSON.decode(Shell.fileGetContents("my-app.v1.0.0.sha512.json"));
var name = "my-app.v1.0.0.win64.bin.zip";
if (Script.isNil(json) || SHA512.fileHash(name) != json[name]) {
	throw(new Error("checksum mismatch: " + name));
};
```

## Write a sha512sum style checksum file

```javascript
Script.requireExtension("Shell");

var files = ["app-1.0.0-win64.zip", "app-1.0.0-linux64.tar.gz"];
var out = "";
for (var name of files) {
	var h = SHA512.fileHash(name);
	if (Script.isUndefined(h)) {
		throw(new Error("cannot hash " + name));
	};
	out += h + " *" + name + "\n";
};
Shell.filePutContents("SHA512SUMS", out);
```

`sha512sum -c SHA512SUMS` checks it.

## Hash every file in a folder

```javascript
Script.requireExtension("Shell");

var list = Shell.getFileList("data/*");
for (var name of list) {
	var h = SHA512.fileHash(name);
	Console.writeLn((Script.isUndefined(h) ? "<error>" : h) + "  " + name);
};
```

## Hash binary data

Strings and buffers are hashed byte for byte, so binary content gives the
same digest as other tools:

```javascript
Script.requireExtension("Shell");

var data = Shell.fileGetContentsBuffer("logo.png");          // Buffer, or undefined
if (data) {
	Console.writeLn(SHA512.hash(data));                       // == SHA512.fileHash("logo.png")
};

var packet = Buffer.fromHex("0102030400ff");
Console.writeLn(SHA512.hash(packet));                         // 6 bytes hashed, zero byte included
```

For large files prefer `SHA512.fileHash`: it reads in chunks instead of
loading the whole file.

## Hash data built in pieces

There is no incremental interface. Join the pieces first:

```javascript
var parts = [header, body, footer];
var digest = SHA512.hash(parts.join(""));
```

The pieces are concatenated without separators, so `["ab", "c"]` and
`["a", "bc"]` give the same digest. When the structure matters (cache keys
built from several fields), add a separator or a length prefix:

```javascript
function cacheKey(fields) {
	var text = "";
	for (var field of fields) {
		var s = "" + field;
		text += s.length + ":" + s + ";";
	};
	return SHA512.hash(text);
};
```

## Content changed? (build stamps, caches)

```javascript
Script.requireExtension("Shell");

var stampFile = "temp/config.json.sha512";
var current = SHA512.fileHash("config.json");
var previous = Shell.fileGetContents(stampFile);
if (current != previous) {
	Console.writeLn("config.json changed, regenerating ...");
	// ... regenerate ...
	Shell.filePutContents(stampFile, current);
};
```

## Shorter keys

A 128 character key is long for file names or URLs. Keep a prefix (64 hex
characters = 256 bits is still far beyond accidental collisions), or encode
the raw digest more compactly:

```javascript
Script.requireExtension("Base64");

var key = SHA512.hash(text).substring(0, 64);
var compact = Base64.encode(SHA512.hashToBuffer(text));   // 88 characters
```

Truncated SHA-512 is **not** SHA-512/256 (a separate algorithm with other
initial values) and not SHA-256; it only matches itself.

## Raw digest, other encodings

```javascript
Script.requireExtension("Base64");

var d = SHA512.hashToBuffer("abc");      // 64 bytes
Console.writeLn(d.toHex());              // hex, same as SHA512.hash("abc")
Console.writeLn(Base64.encode(d));       // base64, as used by Subresource Integrity ("sha512-" + ...)
Console.writeLn(SHA512.hash(d));         // double SHA-512: SHA512(SHA512(x))
```

`Buffer.fromHex(SHA512.hash(x))` and `SHA512.hashToBuffer(x)` hold the same
64 bytes.

## What SHA-512 is not for

SHA-512 is a fast, unkeyed hash. It is the right tool for integrity checks,
content identifiers and cache keys. It is the wrong tool for:

- **Passwords.** A plain `SHA512.hash(password)` (with or without a salt) can
  be brute forced quickly. Use a slow password hash (Argon2, scrypt, bcrypt,
  PBKDF2); no Quantum Script extension provides one, so use a native library
  or an external tool.
- **Message authentication.** `SHA512.hash(secret + message)` is open to
  length extension attacks. Use HMAC-SHA512 instead; this extension does not
  provide it.
- **Encryption.** A hash cannot be reversed; to encrypt data see the `Crypt`
  or `OpenSSL` extensions.
- **Secret comparison.** String `==` is not constant time; do not use it to
  check secret tokens where timing can be observed.
