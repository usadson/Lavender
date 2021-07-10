/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <array>

#include <cmath>

#include "Source/Math/Vector.hpp"

namespace math {

    template <typename Type>
    class Quaternion {
        std::array<Type, 4> m_data{};
    public:
        [[nodiscard]] Quaternion() noexcept = default;
        [[nodiscard]] Quaternion(Quaternion &&) noexcept = default;
        [[nodiscard]] Quaternion(const Quaternion &) noexcept = default;
        Quaternion &operator=(Quaternion &&) noexcept = default;
        Quaternion &operator=(const Quaternion &) noexcept = default;

        [[nodiscard]] inline constexpr
        Quaternion(Type x, Type y, Type z, Type w) noexcept
                : m_data{x, y, z, w} {
        }

        [[nodiscard]] inline constexpr Type
        x() const noexcept {
            return m_data[0];
        }

        [[nodiscard]] inline constexpr Type
        y() const noexcept {
            return m_data[1];
        }

        [[nodiscard]] inline constexpr Type
        z() const noexcept {
            return m_data[2];
        }

        [[nodiscard]] inline constexpr Type
        w() const noexcept {
            return m_data[3];
        }

        [[nodiscard]] inline Quaternion
        conjugate() const noexcept {
            return {-x(), -y(), -z(), w()};
        }

        [[nodiscard]] inline Quaternion
        mul(const Quaternion &other) const noexcept {
            return {
                x() * other.w() + w() * other.x() + y() * other.z() - z() * other.y(),
                y() * other.w() + w() * other.y() + z() * other.x() - x() * other.z(),
                z() * other.w() + w() * other.z() + x() * other.y() - y() * other.x(),
                w() * other.w() - x() * other.x() - y() * other.y() - z() * other.z()
            };
        }

        [[nodiscard]] inline Quaternion
        mul(const Vector<Type, 3> &other) const noexcept {
            return {
                 w() * other.x() + y() * other.z() - z() * other.y(),
                 w() * other.y() + z() * other.x() - x() * other.z(),
                 w() * other.z() + x() * other.y() - y() * other.x(),
                -x() * other.x() - y() * other.y() - z() * other.z(),
            };
        }
    };

} // namespace math
