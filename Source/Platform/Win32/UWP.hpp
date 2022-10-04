/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#if defined(_WINRT_DLL)
#   define LAVENDER_PLATFORM_UWP_AVAILABLE
#else
#   include <sdkddkver.h>
#   if WINVER >= _WIN32_WINNT_WIN10
#       define LAVENDER_PLATFORM_UWP_AVAILABLE
#   endif
#endif

#ifdef LAVENDER_PLATFORM_UWP_AVAILABLE

namespace platform::win32::uwp {

    enum class DarkMode {
        OFF, 
        ON
    };

    [[nodiscard]] DarkMode 
    getDarkModeSetting() noexcept;

} // namespace platform::win32::uwp

#endif // LAVENDER_PLATFORM_UWP_AVAILABLE
