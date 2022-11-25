/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <Windows.h>

#include <string_view>

namespace platform::win32 {

    enum class WindowHandleType {
#define PLATFORM_WIN32_ITERATE_WINDOW_HANDLE_TYPE(REGISTER_TYPE) \
        REGISTER_TYPE(UNKNOWN) \
        REGISTER_TYPE(WINDOW) \

#define REGISTER_TYPE(enumeration) enumeration,
        PLATFORM_WIN32_ITERATE_WINDOW_HANDLE_TYPE(REGISTER_TYPE)
#undef REGISTER_TYPE
    };

    [[nodiscard]] WindowHandleType
    findWindowHandleType(HWND) noexcept;
    
} // namespace platform::win32

[[nodiscard]] inline constexpr std::string_view
toString(platform::win32::WindowHandleType windowHandleType) noexcept {
    switch (windowHandleType) {
#define CHECK_TYPE(enumeration) \
        case platform::win32::WindowHandleType::enumeration: \
            return #enumeration;

        PLATFORM_WIN32_ITERATE_WINDOW_HANDLE_TYPE(CHECK_TYPE)
#undef CHECK_TYPE
    }

    return "(invalid)";
}
