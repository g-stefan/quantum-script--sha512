//
// Quantum Script Extension SHA512
//
// Copyright (c) 2020 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quantum-script-extension-sha512-license.hpp"
#include "quantum-script-extension-sha512.hpp"
#ifndef QUANTUM_SCRIPT_EXTENSION_SHA512_NO_VERSION
#include "quantum-script-extension-sha512-version.hpp"
#endif

#include "quantum-script-variablestring.hpp"
//
#include "quantum-script-extension-buffer-variablebuffer.hpp"

//#define QUANTUM_SCRIPT_VM_DEBUG_RUNTIME

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace SHA512 {

				using namespace XYO;
				using namespace Quantum::Script;

				static TPointer<Variable> hash(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- sha512-hash\n");
#endif
					return VariableString::newVariable(XYO::SHA512::getHashString((arguments->index(0))->toString()));
				};

				static TPointer<Variable> hashToBuffer(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- sha512-hash-to-buffer\n");
#endif
					TPointer<Variable> retV(Extension::Buffer::VariableBuffer::newVariable(64));
					((Extension::Buffer::VariableBuffer *)retV.value())->buffer.length = 64;
					XYO::SHA512::hashStringToU8((arguments->index(0))->toString(), ((Extension::Buffer::VariableBuffer *)retV.value())->buffer.buffer);
					return retV;
				};

				void registerInternalExtension(Executive *executive) {
					executive->registerInternalExtension("SHA512", initExecutive);
				};


				void initExecutive(Executive *executive, void *extensionId) {

					String info = "SHA512\r\n";
					info << License::shortContent();

					executive->setExtensionName(extensionId, "SHA512");
					executive->setExtensionInfo(extensionId, info);
#ifndef QUANTUM_SCRIPT_EXTENSION_SHA512_NO_VERSION
					executive->setExtensionVersion(extensionId, Extension::SHA512::Version::versionWithBuild());
#endif
					executive->setExtensionPublic(extensionId, true);

					executive->compileStringX("Script.requireExtension(\"Buffer\");");
					executive->compileStringX("var SHA512={};");
					executive->setFunction2("SHA512.hash(str)", hash);
					executive->setFunction2("SHA512.hashToBuffer(str)", hashToBuffer);
				};

			};
		};
	};
};

#ifdef XYO_COMPILE_DYNAMIC_LIBRARY
extern "C" QUANTUM_SCRIPT_EXTENSION_SHA512_EXPORT void quantumScriptExtension(Quantum::Script::Executive *executive, void *extensionId) {
	Quantum::Script::Extension::SHA512::initExecutive(executive, extensionId);
};
#endif

