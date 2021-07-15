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

        inline constexpr Matrix4x4 &
        identity() noexcept {
            m_data = std::array<std::array<Type, 4>, 4>{{
                {1, 0, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 1}
            }};
            return *this;
        }

        inline constexpr Matrix4x4 &
        translate(const Vector<Type, 3> &vector) noexcept {
            m_data = std::array<std::array<Type, 4>, 4>{{
                {1, 0, 0, vector.x()},
                {0, 1, 0, vector.y()},
                {0, 0, 1, vector.z()},
                {0, 0, 0, 1}
            }};
            return *this;
        }

        inline constexpr Matrix4x4 &
        rotate(const Vector<Type, 3> &vector) noexcept {
            Matrix4x4<Type> rx, ry, rz;

            const auto x = toRadians(vector.x());
            const auto y = toRadians(vector.y());
            const auto z = toRadians(vector.z());

            rx.m_data = std::array<std::array<Type, 4>, 4>{{
                {1, 0, 0, 0},
                {0, std::cos(x), -std::sin(x), 0},
                {0, std::sin(x), std::cos(x), 0},
                {0, 0, 0, 1}
            }};

            ry.m_data = std::array<std::array<Type, 4>, 4>{{
                {std::cos(y), 0, -std::sin(y), 0},
                {0,           1, 0,            0},
                {std::sin(y), 0, std::cos(y),  0},
                {0,           0, 0,            1}
            }};

            rz.m_data = std::array<std::array<Type, 4>, 4>{{
                {std::cos(z), -std::sin(z), 0, 0},
                {std::sin(z), std::cos(z), 0, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 1}
            }};

            m_data = rz.mul(ry.mul(rx)).m_data;
            return *this;
        }

        inline constexpr Matrix4x4 &
        scale(const Vector<Type, 3> &vector) noexcept {
            m_data = std::array<std::array<Type, 4>, 4>{{
                {vector.x(), 0,          0,          0},
                {0,          vector.y(), 0,          0},
                {0,          0,          vector.z(), 0},
                {0,          0,          0,          1}
            }};
            return *this;
        }

        [[nodiscard]] inline constexpr Matrix4x4
        mul(const Matrix4x4<Type> &other) const noexcept {
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

    [[nodiscard]] Matrix4x4<float>
    createCameraViewMatrix(Vector3f forward, Vector3f up) noexcept;

    [[nodiscard]] Matrix4x4<float>
    createPerspectiveProjectionMatrix(float fov, float width, float height, float zNear, float zFar) noexcept;

} // namespace math
