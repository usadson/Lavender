/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Source/Math/Vector.hpp"

namespace ecs {

    struct PointLight {
        math::Vector3f position;
        math::Vector3f color;
        float radius;
        float intensity;
        float attenuationConstant;
        float attenuationLinear;
        float attenuationExponent;
    };

} // namespace ecs