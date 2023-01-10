// Quantum Script Extension SHA512
// Copyright (c) 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_SHA512_VERSION_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_SHA512_VERSION_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_SHA512_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/SHA512/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::SHA512::Version {

	XYO_QUANTUMSCRIPT_EXTENSION_SHA512_EXPORT const char *version();
	XYO_QUANTUMSCRIPT_EXTENSION_SHA512_EXPORT const char *build();
	XYO_QUANTUMSCRIPT_EXTENSION_SHA512_EXPORT const char *versionWithBuild();
	XYO_QUANTUMSCRIPT_EXTENSION_SHA512_EXPORT const char *datetime();

};

#endif
