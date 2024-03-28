// Quantum Script Extension SHA512
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/SHA512/Library.hpp>
#include <XYO/QuantumScript.Extension/SHA512/Copyright.hpp>
#include <XYO/QuantumScript.Extension/SHA512/License.hpp>
#include <XYO/QuantumScript.Extension/SHA512/Version.hpp>
#include <XYO/QuantumScript.Extension/Buffer.hpp>
#include <XYO/Cryptography.hpp>

namespace XYO::QuantumScript::Extension::SHA512 {

	static TPointer<Variable> hash(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- sha512-hash\n");
#endif
		return VariableString::newVariable(XYO::Cryptography::SHA512::hash((arguments->index(0))->toString()));
	};

	static TPointer<Variable> hashToBuffer(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- sha512-hash-to-buffer\n");
#endif
		TPointer<Variable> retV(Extension::Buffer::VariableBuffer::newVariable(64));
		((Extension::Buffer::VariableBuffer *)retV.value())->buffer.length = 64;
		XYO::Cryptography::SHA512::hashToU8((arguments->index(0))->toString(), ((Extension::Buffer::VariableBuffer *)retV.value())->buffer.buffer);
		return retV;
	};

	static TPointer<Variable> fileHash(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- sha512-file-hash\n");
#endif
		String retVal;
		if (XYO::Cryptography::Util::fileHashSHA512((arguments->index(0))->toString(), retVal)) {
			return VariableString::newVariable(retVal);
		};
		return Context::getValueUndefined();
	};

	void registerInternalExtension(Executive *executive) {
		executive->registerInternalExtension("SHA512", initExecutive);
	};

	void initExecutive(Executive *executive, void *extensionId) {

		String info = "SHA512\r\n";
		info << License::shortLicense().c_str();

		executive->setExtensionName(extensionId, "SHA512");
		executive->setExtensionInfo(extensionId, info);
		executive->setExtensionVersion(extensionId, Extension::SHA512::Version::versionWithBuild());
		executive->setExtensionPublic(extensionId, true);

		executive->compileStringX("Script.requireExtension(\"Buffer\");");
		executive->compileStringX("var SHA512={};");
		executive->setFunction2("SHA512.hash(str)", hash);
		executive->setFunction2("SHA512.hashToBuffer(str)", hashToBuffer);
		executive->setFunction2("SHA512.fileHash(filename)", fileHash);
	};

};

#ifdef XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY
extern "C" XYO_QUANTUMSCRIPT_EXTENSION_SHA512_EXPORT void quantumScriptExtension(XYO::QuantumScript::Executive *executive, void *extensionId) {
	XYO::QuantumScript::Extension::SHA512::initExecutive(executive, extensionId);
};
#endif
