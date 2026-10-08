# C++ API

```cpp
#include <XYO/QuantumScript.Extension/SHA512.hpp>

namespace XYO::QuantumScript::Extension::SHA512 {
	void initExecutive(Executive *executive, void *extensionId);
	void registerInternalExtension(Executive *executive);
};
```

The extension exposes only what a host needs to load it. The hashing is done
by `xyo-cryptography`; C++ code that wants SHA-512 calls that library
directly (section 4) instead of going through a script.

## 1. registerInternalExtension

```cpp
void registerInternalExtension(Executive *executive);
```

Registers `SHA512` as an internal extension of `executive`
(`executive->registerInternalExtension("SHA512", initExecutive)`). Call it in
the host's init callback passed to `ExecutiveX::initExecutive`, after
registering `Buffer`:

```cpp
void initExecutive(Executive *executive) {
	Extension::Console::registerInternalExtension(executive);
	Extension::Buffer::registerInternalExtension(executive);
	Extension::SHA512::registerInternalExtension(executive);
};
```

Nothing runs until a script calls `Script.requireExtension("SHA512")` (or
`requireInternalExtension`); then `initExecutive` is called once for that
executive.

## 2. initExecutive

```cpp
void initExecutive(Executive *executive, void *extensionId);
```

The extension's initialization, called by the engine when the extension is
loaded. It:

1. sets the extension name (`SHA512`), info (name plus short MIT license),
   version (`Version::versionWithBuild()`) and marks it public;
2. compiles `Script.requireExtension("Buffer");` — `hashToBuffer` returns a
   `VariableBuffer`, which needs the per thread `Buffer` prototype;
3. compiles `var SHA512={};`;
4. registers the native functions:

```cpp
executive->setFunction2("SHA512.hash(str)", hash);
executive->setFunction2("SHA512.hashToBuffer(str)", hashToBuffer);
executive->setFunction2("SHA512.fileHash(filename)", fileHash);
```

In the DLL build the exported C entry point forwards to it:

```cpp
extern "C" void quantumScriptExtension(XYO::QuantumScript::Executive *executive, void *extensionId);
```

The entry point is compiled only when `XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY`
is defined and `XYO_QUANTUMSCRIPT_EXTENSION_SHA512_LIBRARY` is not.

## 3. Export macros and metadata

`SHA512/Dependency.hpp` defines `XYO_QUANTUMSCRIPT_EXTENSION_SHA512_EXPORT`:

| Define | Effect |
|--------|--------|
| `XYO_QUANTUMSCRIPT_EXTENSION_SHA512_INTERNAL` (or `QUANTUM_SCRIPT__SHA512_INTERNAL`) | building the DLL: symbols exported |
| none | using the DLL: symbols imported |
| `XYO_QUANTUMSCRIPT_EXTENSION_SHA512_LIBRARY` | static use: export macro empty, no `quantumScriptExtension` entry point; set for consumers of `quantum-script--sha512.static` |

Library metadata, generated from `version.json` and the project info:

```cpp
namespace XYO::QuantumScript::Extension::SHA512::Version {
	const char *version();            // for example "5.9.0"
	const char *build();              // for example "6"
	const char *versionWithBuild();   // for example "5.9.0.6"
	const char *datetime();           // for example "2026-09-16 23:03:10"
};
namespace XYO::QuantumScript::Extension::SHA512::License {
	std::string license();
	std::string shortLicense();
};
namespace XYO::QuantumScript::Extension::SHA512::Copyright {
	const char *copyright();
	const char *publisher();
	const char *company();
	const char *contact();
};
```

## 4. SHA-512 from C++ (xyo-cryptography)

The functions behind the script API, usable from any C++ code that depends on
`xyo-cryptography`:

```cpp
#include <XYO/Cryptography.hpp>

using namespace XYO;

// one shot, hex: what SHA512.hash does
String hex = Cryptography::SHA512::hash("abc");     // 128 lowercase hex characters

// one shot, raw: what SHA512.hashToBuffer does
uint8_t digest[64];
Cryptography::SHA512::hashToU8("abc", digest);

// a file: what SHA512.fileHash does
String fileHex;
if (!Cryptography::Util::fileHashSHA512("archive.zip", fileHex)) {
	// cannot open, seek, or read the whole file
};

// incremental
Cryptography::SHA512 sha;                          // constructor calls processInit()
sha.processU8((const uint8_t *)part1, part1Size);
sha.processU8((const uint8_t *)part2, part2Size);
sha.processDone();                                 // padding and length, once
String hex2 = sha.getHashHex();
sha.toU8(digest);                                  // same digest, 64 raw bytes
sha.processInit();                                 // reset before reusing the object
```

`String` arguments are hashed over `length()` bytes, zero bytes included.
`processDone()` finalizes the state: call it exactly once, then read the
result with `getHashHex()` / `toU8()`, and call `processInit()` before
hashing new data with the same object. `copy(other)` duplicates a state,
useful to hash several messages that share a prefix.

## 5. Returning a digest from your own native function

The `hashToBuffer` pattern, for an extension that returns raw bytes (requires
`quantum-script--buffer` and `Script.requireExtension("Buffer")` in its
`initExecutive`):

```cpp
#include <XYO/QuantumScript.Extension/Buffer.hpp>
#include <XYO/Cryptography.hpp>

static TPointer<Variable> digestOf(VariableFunction *function, Variable *this_, VariableArray *arguments) {
	TPointer<Variable> retV(Extension::Buffer::VariableBuffer::newVariable(64));   // size 64, length 0
	Extension::Buffer::VariableBuffer *buffer = (Extension::Buffer::VariableBuffer *)retV.value();
	XYO::Cryptography::SHA512::hashToU8((arguments->index(0))->toString(), buffer->buffer.buffer);
	buffer->buffer.length = 64;
	return retV;
};
```

Returning hex text instead:

```cpp
return VariableString::newVariable(XYO::Cryptography::SHA512::hash((arguments->index(0))->toString()));
```

Returning `undefined` on failure, as `fileHash` does:

```cpp
return Context::getValueUndefined();
```
