/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <array>

#include <cmath>
#include <cstdint>

#include "Source/Math/Math.hpp"
#include "Source/Math/Vector.hpp"

namespace math {

    template<typename Type>
    class Matrix4x4 {
    public:
        std::array<std::array<Type, 4>, 4> m_data{};

        [[nodiscard]] inline constexpr std::array<Type, 4> &
        operator[](std::size_t x) noexcept {
            return m_data[x];
        }

        [[nodiscard]] inline constexpr const std::array<Type, 4> &
        operator[](std::size_t x) const noexcept {
            return m_data[x];
        }

        [[nodiscard]] inline constexpr Matrix4x4 &
        identity() noexcept {
            m_data = std::array<std::array<Type, 4>, 4>{{
                {1, 0, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 1}
            }};
            return *this;
        }

        [[nodiscard]] inline constexpr Matrix4x4 &
        translate(const Vector<Type, 3> &vector) noexcept {
            m_data = std::array<std::array<Type, 4>, 4>{{
                {1, 0, 0, vector.x()},
                {0, 1, 0, vector.y()},
                {0, 0, 1, vector.z()},
                {0, 0, 0, 1}
            }};
            return *this;
        }

        [[nodiscard]] inline constexpr Matrix4x4 &
        mul(const Matrix4x4<Type> &other) noexcept {
            Matrix4x4<Type> result{};

            for (std::size_t x = 0; x < 4; ++x) {
                for (std::size_t y = 0; y < 4; ++y) {
                    result[x][y] = m_data[x][0] * other[0][y]
                                 + m_data[x][1] * other[1][y]
                                 + m_data[x][2] * other[2][y]
                                 + m_data[x][3] * other[3][y];
                }
            }

            return result;
        }
    };

} // namespace math
