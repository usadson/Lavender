/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#include "Input.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Source/Platform/Win32/VirtualKey.hpp"

namespace platform::win32 {

    std::optional<input::KeyboardKey>
    Input::translateKey(std::uintmax_t input) noexcept {
        switch (input) { 

#define CHECK_KEY(enumerator, winMacro, inputKey) \
        case winMacro: \
            return inputKey;

            PLATFORM_WIN32_ITERATE_VIRTUAL_KEYS(CHECK_KEY)
        }

        return std::nullopt;
    }

} // namespace platform::win32
