# Getting started

## 1. Build and install

The extension is built with [fabricare](https://github.com/g-stefan/fabricare),
the build tool used by all XYO C++ projects. `quantum-script` (and everything
below it: `xyo-system`, `xyo-encoding`, ...), `quantum-script--console`,
`quantum-script--buffer` and `xyo-cryptography` must be installed to the SDK
first. From the repository root:

```bash
fabricare make       # build into output/
fabricare test       # build and run test/test.01 (run make first)
fabricare install    # copy output/{bin,include,lib} to ~/.fabricare/<platform>
fabricare clean      # remove output/ and temp/
```

`fabricare.json` has two library projects:

| Project | Make | Result | Use it when |
|---------|------|--------|-------------|
| `quantum-script--sha512` | `dll-or-lib` | `quantum-script--sha512.dll` / `libquantum-script--sha512.so` on dynamic platforms, a static library on static ones | scripts run by `quantum-script`, or a host using the engine DLL |
| `quantum-script--sha512.static` | `lib`, static CRT | static library, defines `XYO_QUANTUMSCRIPT_EXTENSION_SHA512_LIBRARY` for its consumers | self-contained executables that register the extension as internal (fabricare itself) |

After `fabricare install` on a dynamic platform the DLL sits in the SDK `bin`
folder next to `quantum-script.exe`, which is where
`Script.requireExtension("SHA512")` finds it.

## 2. Use it from a script

```javascript
Script.requireExtension("Console");
Script.requireExtension("SHA512");

Console.writeLn(SHA512.hash("abc"));
// ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f

var digest = SHA512.hashToBuffer("abc");
Console.writeLn(digest.length);                  // 64
Console.writeLn(digest.toHex());                 // same text as SHA512.hash("abc")

var fileDigest = SHA512.fileHash("hello.js");
if (Script.isUndefined(fileDigest)) {
	Console.writeLn("cannot read hello.js");
} else {
	Console.writeLn(fileDigest + "  hello.js");
};
```

Run it with:

```bash
quantum-script hello-sha512.js
```

`Script.requireExtension("SHA512")` looks for an external
`quantum-script--sha512` library first (the file as named, then every include
path folder: next to the interpreter, next to the script), then for an
internal extension registered by the host. Loading twice does nothing. A
missing extension throws `Unable to open "SHA512"`.

Loading `SHA512` also loads `Buffer` (the extension runs
`Script.requireExtension("Buffer")` while it initializes), so the `Buffer`
global exists afterwards.

### fabricare build scripts

`fabricare` is a static executable that embeds `SHA512` (it depends on
`quantum-script--sha512.static` and registers it as internal), so build
scripts can use it directly:

```javascript
Script.requireExtension("SHA512");

var h = SHA512.fileHash("release/my-app.v1.0.0.win64.bin.zip");
```

fabricare's own `Library.js` already requires it, and its `release` /
`gitea-release` actions use `SHA512.fileHash` to write
`release/<project>.v<version>.sha512.json` (see [Recipes](recipes.md)).
fabricare cannot load extension DLLs, and it does **not** embed `SHA256` or
`MD5`: in fabricare scripts SHA-512 is the hash to use.

## 3. Register it in a C++ host

A host that embeds Quantum Script makes `SHA512` available as an internal
extension by registering it, together with `Buffer`, in the init callback
(this is what `test/test.01.cpp` does):

```cpp
#include <XYO/QuantumScript.hpp>
#include <XYO/QuantumScript.Extension/Console.hpp>
#include <XYO/QuantumScript.Extension/Buffer.hpp>
#include <XYO/QuantumScript.Extension/SHA512.hpp>

using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {
	Extension::Console::registerInternalExtension(executive);
	Extension::Buffer::registerInternalExtension(executive);
	Extension::SHA512::registerInternalExtension(executive);
};

int main(int cmdN, char *cmdS[]) {
	if (ExecutiveX::initExecutive(cmdN, cmdS, initExecutive)) {
		if (!ExecutiveX::executeString(
		        "Script.requireExtension(\"Console\");"
		        "Script.requireExtension(\"SHA512\");"
		        "Console.writeLn(SHA512.hash(\"abc\"));")) {
			printf("%s\n", (ExecutiveX::getError()).value());
			printf("%s", (ExecutiveX::getStackTrace()).value());
		};
		ExecutiveX::endProcessing();
	};
	return 0;
};
```

`Buffer` must be registered too (or be loadable as a DLL): `SHA512` requires
it during its own initialization.

Registering only makes the extension *available*: scripts still call
`Script.requireExtension("SHA512")`. With the DLL build of the engine an
external `quantum-script--sha512.dll` found on the include path wins over the
internal one for `requireExtension`; use
`Script.requireInternalExtension("SHA512")` to force the internal one.

In the host's `fabricare.json`:

```json
{
	"name": "my-host",
	"make": "exe",
	"sourcePath": "XYO/MyHost",
	"dependency": [
		"quantum-script--sha512"
	]
}
```

`quantum-script--sha512` brings `quantum-script`, `quantum-script--console`,
`quantum-script--buffer` and `xyo-cryptography` with it.
`quantum-script--magnet` is an example of a host that registers `SHA512`
this way.

## 4. Static builds

For a self-contained executable depend on the static variants and the static
CRT:

```json
{
	"name": "my-host.static",
	"make": "exe",
	"sourcePath": "XYO/MyHost",
	"dependency": [
		"quantum-script.static",
		"quantum-script--console.static",
		"quantum-script--buffer.static",
		"quantum-script--sha512.static"
	],
	"crt": "static"
}
```

`quantum-script--sha512.static` exports the define
`XYO_QUANTUMSCRIPT_EXTENSION_SHA512_LIBRARY` to its consumers, which turns
`XYO_QUANTUMSCRIPT_EXTENSION_SHA512_EXPORT` into nothing and leaves out the
`quantumScriptExtension` DLL entry point. A static host must register the
extension (and `Buffer`) with `registerInternalExtension` (section 3):
external DLLs cannot be loaded into a host that does not use the engine DLL.
fabricare is built exactly this way.

## 5. Threads

Each thread that runs scripts has its own engine, so every thread loads the
extension itself with `Script.requireExtension("SHA512")`. The functions keep
no state between calls; each call creates its own hash context, so there is
nothing to share or lock.
