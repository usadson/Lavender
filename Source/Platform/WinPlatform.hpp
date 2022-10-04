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
#   error "Win32 support not enabled! This file shouldn't have been included."
#endif

#define _WINCON_ // prevent wincon.h inclusion
#include <minwindef.h>

#include "Source/Base/Library.hpp"
#include "Source/Platform/Win32/WindowNotifications.hpp"

namespace platform::win32 {

    extern HINSTANCE g_hInstance;

} // namespace platform::win32
