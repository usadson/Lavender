/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

namespace platform::win32::undocumented {

    [[nodiscard]] DWORD
    getImmersiveColorFromColorSetEx(UINT colorSet, UINT colorType, bool ignoreHighContrast, UINT highContrastCacheMode);

	[[nodiscard]] WCHAR **
    getImmersiveColorNamedTypeByIndex(UINT);

	[[nodiscard]] int 
	getImmersiveColorSetCountType();

	[[nodiscard]] int
	getImmersiveUserColorSetPreference(bool forceCheckRegistry, bool skipCheckOnFail);

	bool
	setConsoleIcon(HICON icon) noexcept;

} // namespace platform::win32::undocumented
