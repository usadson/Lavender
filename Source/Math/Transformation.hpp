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
        Vector<float, 3> translation;

        [[nodiscard]] Matrix4x4<float>
        toMatrix() const noexcept {
            // TODO maybe its better to only calculate this when something
            //      changes, and cache the result of that.

            return Matrix4x4<float>().translate(translation);
        }
    };

} // namespace math
