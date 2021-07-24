/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#pragma once

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

} // namespace gle
