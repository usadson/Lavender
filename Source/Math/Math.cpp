/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Source/Math/Math.hpp"
#include "Source/Math/Vector.hpp"
#include "Source/Math/Quaternion.hpp"

namespace math {

    Vector3f
    rotateV3f(Vector3f input, float angleDegrees, Vector3f axis) noexcept {
        const auto rad = toRadians(angleDegrees / 2);
        const auto sin = static_cast<float>(std::sin(rad));
        const auto cos = static_cast<float>(std::cos(rad));

        math::Quaternion<float> rotation{
            axis.x() * sin,
            axis.y() * sin,
            axis.z() * sin,
            cos
        };

        auto conjugate = rotation.conjugate();
        auto w = rotation.mul(input).mul(conjugate);

        return {w.x(), w.y(), w.z()};
    }

} // namespace math
