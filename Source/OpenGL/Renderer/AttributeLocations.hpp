/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#pragma once

#include "Source/OpenGL/Types.hpp"

namespace gle {

    struct AttributeLocations {
        GL::UnsignedIntType normal{};
        GL::UnsignedIntType position{};
        GL::UnsignedIntType textureCoordinates{};
    };

} // namespace gle
