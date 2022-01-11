//
// Quantum Script Extension SHA512
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_SHA512_VERSION_HPP
#define QUANTUM_SCRIPT_EXTENSION_SHA512_VERSION_HPP

#define QUANTUM_SCRIPT_EXTENSION_SHA512_VERSION_ABCD                 2,12,0,24
#define QUANTUM_SCRIPT_EXTENSION_SHA512_VERSION_STR                 "2.12.0"
#define QUANTUM_SCRIPT_EXTENSION_SHA512_VERSION_STR_BUILD           "24"
#define QUANTUM_SCRIPT_EXTENSION_SHA512_VERSION_STR_DATETIME        "2022-01-09 01:13:12"

#ifndef XYO_RC

#ifndef QUANTUM_SCRIPT_EXTENSION_SHA512__EXPORT_HPP
#include "quantum-script-extension-sha512--export.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace SHA512 {
				namespace Version {
					QUANTUM_SCRIPT_EXTENSION_SHA512_EXPORT const char *version();
					QUANTUM_SCRIPT_EXTENSION_SHA512_EXPORT const char *build();
					QUANTUM_SCRIPT_EXTENSION_SHA512_EXPORT const char *versionWithBuild();
					QUANTUM_SCRIPT_EXTENSION_SHA512_EXPORT const char *datetime();
				};
			};
		};
	};
};

#endif
#endif

