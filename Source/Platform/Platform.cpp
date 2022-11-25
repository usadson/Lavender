/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#include "Source/Platform/Platform.hpp"

#ifdef LAVENDER_WIN32_SUPPORT_ENABLED
#include "Source/Platform/Win32/Platform.hpp"
#endif

namespace platform {

    base::ErrorOr<std::unique_ptr<Platform>>
    Platform::create() noexcept {
#ifdef LAVENDER_WIN32_SUPPORT_ENABLED
        return {std::make_unique<win32::Win32Platform>()};
#else
        return base::Error(
            "PlatformCore", "PlatformCore", "Create UserInteraction interface", "This platform is not supported.");
#endif  
    }

} // namespace platform
