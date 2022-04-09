//
// Quantum Script Extension SHA512
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_SHA512_HPP
#define QUANTUM_SCRIPT_EXTENSION_SHA512_HPP

#ifndef QUANTUM_SCRIPT_HPP
#	include "quantum-script.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_SHA512__EXPORT_HPP
#	include "quantum-script-extension-sha512--export.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_SHA512_COPYRIGHT_HPP
#	include "quantum-script-extension-sha512-copyright.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_SHA512_LICENSE_HPP
#	include "quantum-script-extension-sha512-license.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_SHA512_VERSION_HPP
#	include "quantum-script-extension-sha512-version.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace SHA512 {

				using namespace Quantum::Script;

				QUANTUM_SCRIPT_EXTENSION_SHA512_EXPORT void initExecutive(Executive *executive, void *extensionId);
				QUANTUM_SCRIPT_EXTENSION_SHA512_EXPORT void registerInternalExtension(Executive *executive);

			};
		};
	};
};

#endif
