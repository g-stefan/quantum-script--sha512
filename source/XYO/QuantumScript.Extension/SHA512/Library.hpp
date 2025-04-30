// Quantum Script Extension SHA512
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_SHA512_LIBRARY_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_SHA512_LIBRARY_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_SHA512_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/SHA512/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::SHA512 {

	XYO_QUANTUMSCRIPT_EXTENSION_SHA512_EXPORT void initExecutive(Executive *executive, void *extensionId);
	XYO_QUANTUMSCRIPT_EXTENSION_SHA512_EXPORT void registerInternalExtension(Executive *executive);

};

#endif
