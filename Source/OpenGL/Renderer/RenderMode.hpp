/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#pragma once

#include <string_view>

#include "Source/Base/Debug.hpp"

namespace gle {

    enum class RenderMode {
        DEFAULT,

#ifdef LAVENDER_BUILD_DEBUG
        DEBUG_NORMALS,
        DEBUG_POSITION,
        DEBUG_COLOR,
#endif
    };

    [[nodiscard]] inline constexpr std::string_view
    toString(RenderMode renderMode) noexcept {
        switch (renderMode) {
        case RenderMode::DEFAULT: return "default";
#ifdef LAVENDER_BUILD_DEBUG
        case RenderMode::DEBUG_NORMALS: return "debug-normals";
        case RenderMode::DEBUG_POSITION: return "debug-position";
        case RenderMode::DEBUG_COLOR: return "debug-color";
#endif
        default: return "(invalid)";
        }
    }

} // namespace gle
