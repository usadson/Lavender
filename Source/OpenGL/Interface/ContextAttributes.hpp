/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
* All Rights Reserved.
 */

#pragma once

namespace gle {

    enum class ContextAttributes {
        END_OF_ATTRIBUTE_SEQUENCE = 0,

        MAJOR_VERSION_ARB = 0x2091,
        MINOR_VERSION_ARB = 0x2092,
        LAYER_PLANE_ARB = 0x2093,
        FLAGS_ARB = 0x2094,
        PROFILE_MASK_ARB = 0x912
    };

} // namespace gle
