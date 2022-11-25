/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#ifdef LAVENDER_WIN32_HAS_OLE
#   pragma error("LAVENDER_WIN32_HAS_OLE already defined")
#endif // LAVENDER_WIN32_HAS_OLE

#if __has_include("Ole2.h")
#define LAVENDER_WIN32_HAS_OLE

#include <Windows.h>
#include <Ole2.h>

#include <string_view>

namespace platform::win32::ole {

    void
    debugPrintClipFormat(CLIPFORMAT) noexcept;

    [[nodiscard]] std::string_view
    oleStatusToString(HRESULT) noexcept;

    [[nodiscard]] std::string_view
    oleTymedToString(TYMED) noexcept;
    
} // namespace platform::win32::ole

#endif // __has_include("Ole2.h")
