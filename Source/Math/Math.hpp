/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <cmath>

namespace math {

    template <typename Type>
    [[nodiscard]] inline constexpr Type
    toDegrees(Type radians) noexcept {
        return (radians * 180) / M_PI;
    }

    template <typename Type>
    [[nodiscard]] inline constexpr Type
    toRadians(Type degrees) noexcept {
        return (degrees * M_PI) / 180;
    }

} // namespace math
