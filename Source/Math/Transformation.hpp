/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Source/Math/Matrix4x4.hpp"
#include "Source/Math/Vector.hpp"

namespace math {

    struct Transformation {
        Vector<float, 3> translation{};
        Vector<float, 3> rotation{};
        Vector<float, 3> scaling{1.0f, 1.0f, 1.0f};

        [[nodiscard]] Matrix4x4<float>
        toMatrix() const noexcept {
            // TODO maybe its better to only calculate this when something
            //      changes, and cache the result of that.

            const auto translationMatrix = Matrix4x4<float>().translate(translation);
            const auto rotationMatrix = Matrix4x4<float>().rotate(rotation);
            const auto scalingMatrix = Matrix4x4<float>().scale(scaling);

            return translationMatrix.mul(rotationMatrix.mul(scalingMatrix));
        }
    };

} // namespace math
