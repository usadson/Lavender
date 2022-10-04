/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#ifndef LAVENDER_WIN32_SUPPORT_ENABLED
#error "Win32 support not enabled! This file shouldn't have been included."
#endif

#include <dwmapi.h>

namespace platform::win32::dwm {

    enum class WindowAttribute : unsigned int {

#define PLATFORM_WIN32_ITERATE_DWM_WINDOW_ATTRIBUTES(REGISTER_WINDOW_ATTRIBUTE) \
        REGISTER_WINDOW_ATTRIBUTE(USE_HOST_BACKDROP_BRUSH, 16) \
        REGISTER_WINDOW_ATTRIBUTE(USE_IMMERSIVE_DARK_MODE, 20) \

#define PLATFORM_WIN32_DWM_REGISTER_WINDOW_ATTRIBUTE(name, macro) name = macro,
        PLATFORM_WIN32_ITERATE_DWM_WINDOW_ATTRIBUTES(PLATFORM_WIN32_DWM_REGISTER_WINDOW_ATTRIBUTE)
#undef PLATFORM_WIN32_DWM_REGISTER_WINDOW_ATTRIBUTE
    };

} // namespace platform::win32::dwm
