/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <cmath>

#if defined(_WIN32) || !defined(M_PI)
#   include <numbers>
#   define LAVENDER_MATH_USE_MODERN
#endif

namespace math {

    template <typename Type>
    [[nodiscard]] inline constexpr
    Type pi() noexcept {
#ifdef LAVENDER_MATH_USE_MODERN
        return std::numbers::pi_v<Type>;
#else
        return static_cast<Type>(M_PI);
#endif
    }

    template <typename Type>
    [[nodiscard]] inline constexpr Type
    toDegrees(Type radians) noexcept {
        return (radians * 180) / pi<Type>();
    }

    template <typename Type>
    [[nodiscard]] inline constexpr Type
    toRadians(Type degrees) noexcept {
        return (degrees * pi<Type>()) / 180;
    }

} // namespace math
