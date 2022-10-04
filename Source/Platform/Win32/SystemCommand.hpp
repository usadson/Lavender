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

#include <WinUser.h>

#include <string_view>

namespace platform::win32 {

    enum class SystemCommand : WPARAM {

#define PLATFORM_WIN32_ITERATE_SYSTEM_COMMANDS(REGISTER_SYSTEM_COMMAND) \
        REGISTER_SYSTEM_COMMAND(CLOSE, SC_CLOSE) \
        REGISTER_SYSTEM_COMMAND(CONTEXT_HELP, SC_CONTEXTHELP) \
        REGISTER_SYSTEM_COMMAND(DEFAULT, SC_DEFAULT) \
        REGISTER_SYSTEM_COMMAND(HOTKEY, SC_HOTKEY) \
        REGISTER_SYSTEM_COMMAND(SCROLL_HORIZONTALLY, SC_HSCROLL) \
        REGISTER_SYSTEM_COMMAND(SCROLL_VERTICALLY, SC_VSCROLL) \
        REGISTER_SYSTEM_COMMAND(SCREEN_SAVER_IS_SECURE, SCF_ISSECURE) \
        REGISTER_SYSTEM_COMMAND(KEY_MENU, SC_KEYMENU) \
        REGISTER_SYSTEM_COMMAND(MAXIMIZE, SC_MAXIMIZE) \
        REGISTER_SYSTEM_COMMAND(MINIMIZE, SC_MINIMIZE) \
        REGISTER_SYSTEM_COMMAND(MONITOR_POWER_CHANGE, SC_MONITORPOWER) \
        REGISTER_SYSTEM_COMMAND(MOUSE_MENU, SC_MOUSEMENU) \
        REGISTER_SYSTEM_COMMAND(MOVE, SC_MOVE) \
        REGISTER_SYSTEM_COMMAND(NEXT_WINDOW, SC_NEXTWINDOW) \
        REGISTER_SYSTEM_COMMAND(PREVIOUS_WINDOW, SC_PREVWINDOW) \
        REGISTER_SYSTEM_COMMAND(RESTORE_WINDOW_DIMENSIONS, SC_RESTORE) \
        REGISTER_SYSTEM_COMMAND(ACTIVATE_SCREENSAVER, SC_SCREENSAVE) \
        REGISTER_SYSTEM_COMMAND(RESIZE, SC_SIZE) \
        REGISTER_SYSTEM_COMMAND(ACTIVATE_START_MENU, SC_TASKLIST) \

#define PLATFORM_WIN32_DWM_REGISTER_SYSTEM_COMMAND(name, macro) name = macro,
        PLATFORM_WIN32_ITERATE_SYSTEM_COMMANDS(PLATFORM_WIN32_DWM_REGISTER_SYSTEM_COMMAND)
#undef PLATFORM_WIN32_DWM_REGISTER_SYSTEM_COMMAND
    };

} // namespace platform::win32

[[nodiscard]] inline constexpr 
std::string_view toString(platform::win32::SystemCommand systemCommand) noexcept {
    switch (systemCommand) {
#define PLATFORM_WIN32_DWM_REGISTER_SYSTEM_COMMAND(name, macro) \
        case platform::win32::SystemCommand::name: \
            return #name;

        PLATFORM_WIN32_ITERATE_SYSTEM_COMMANDS(PLATFORM_WIN32_DWM_REGISTER_SYSTEM_COMMAND)
#undef PLATFORM_WIN32_DWM_REGISTER_SYSTEM_COMMAND
    }

    return "(invalid)";
}
