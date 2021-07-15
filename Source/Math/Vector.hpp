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

namespace math {

    template <typename Type, std::size_t Dimensions>
        requires(Dimensions >= 2)
    class Vector {
        std::array<Type, Dimensions> m_data{};

    public:
        [[nodiscard]] Vector() = default;
        [[nodiscard]] Vector(Vector &&) = default;
        [[nodiscard]] Vector(const Vector &) = default;
        Vector &operator=(Vector &&) = default;
        Vector &operator=(const Vector &) = default;

        template <typename... CType>
        [[nodiscard]] inline constexpr
        Vector(CType... values) noexcept
            : m_data{(values)...} {
        }

        [[nodiscard]] inline constexpr Type &
        x() noexcept {
            return m_data[0];
        }

        [[nodiscard]] inline constexpr Type &
        y() noexcept {
            return m_data[1];
        }

        [[nodiscard]] inline constexpr Type &
        z() noexcept
        requires(Dimensions >= 3) {
            return m_data[2];
        }

        [[nodiscard]] inline constexpr Type &
        w() noexcept
        requires(Dimensions >= 4) {
            return m_data[3];
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
        z() const noexcept
                requires(Dimensions >= 3) {
            return m_data[2];
        }

        [[nodiscard]] inline constexpr Type
        w() const noexcept
                requires(Dimensions >= 4) {
            return m_data[3];
        }

        [[nodiscard]] inline Vector<Type, Dimensions>
        add(const Vector<Type, Dimensions> &other) const noexcept {
            Vector result(*this);
            for (std::size_t i = 0; i < Dimensions; ++i)
                result.m_data[i] += other.m_data[i];
            return result;
        }

        [[nodiscard]] inline Vector<Type, Dimensions>
        add(Type value) const noexcept {
            Vector result(*this);
            for (std::size_t i = 0; i < Dimensions; ++i)
                result.m_data[i] += value;
            return result;
        }

        [[nodiscard]] inline Vector<Type, Dimensions>
        subtract(const Vector<Type, Dimensions> &other) const noexcept {
            Vector result(*this);
            for (std::size_t i = 0; i < Dimensions; ++i)
                result.m_data[i] -= other.m_data[i];
            return result;
        }

        [[nodiscard]] inline Vector<Type, Dimensions>
        sub(Type value) const noexcept {
            Vector result(*this);
            for (std::size_t i = 0; i < Dimensions; ++i)
                result.m_data[i] -= value;
            return result;
        }

        [[nodiscard]] inline Vector<Type, Dimensions>
        mul(const Vector<Type, Dimensions> &other) const noexcept {
            Vector result(*this);
            for (std::size_t i = 0; i < Dimensions; ++i)
                result.m_data[i] *= other.m_data[i];
            return result;
        }

        [[nodiscard]] inline Vector<Type, Dimensions>
        mul(Type value) const noexcept {
            Vector result(*this);
            for (std::size_t i = 0; i < Dimensions; ++i)
                result.m_data[i] *= value;
            return result;
        }

        [[nodiscard]] inline Vector<Type, Dimensions>
        div(const Vector<Type, Dimensions> &other) const noexcept {
            Vector result(*this);
            for (std::size_t i = 0; i < Dimensions; ++i)
                result.m_data[i] /= other.m_data[i];
            return result;
        }

        [[nodiscard]] inline Vector<Type, Dimensions>
        div(Type value) const noexcept {
            Vector result(*this);
            for (std::size_t i = 0; i < Dimensions; ++i)
                result.m_data[i] /= value;
            return result;
        }

        [[nodiscard]] inline Vector<Type, 3>
        cross(const Vector<Type, 3> &other) const noexcept
                requires(Dimensions == 3) {
            const auto cx = y() * other.z() - z() * other.y();
            const auto cy = z() * other.x() - x() * other.z();
            const auto cz = x() * other.y() - y() * other.x();
            return Vector<Type, 3>(cx, cy, cz);
        }

        [[nodiscard]] inline Type
        dot(const Vector<Type, Dimensions> &other) const noexcept {
            Type total = 0;
            for (std::size_t i = 0; i < Dimensions; ++i)
                total += this->m_data[i] * other.m_data[i];
            return total;
        }

        [[nodiscard]] inline Type
        length() const noexcept {
            Type total = 0;
            for (std::size_t i = 0; i < Dimensions; ++i)
                total += m_data[i] * m_data[i];
            return std::sqrt(total);
        }

        [[nodiscard]] inline Vector<Type, Dimensions> &
        normalize() noexcept {
            const auto length = this->length();

            for (std::size_t i = 0; i < Dimensions; ++i)
                m_data[i] /= length;

            return *this;
        }

        [[nodiscard]] inline Vector<Type, Dimensions>
        normalizeCopy() const noexcept {
            return Vector(*this).normalize();
        }

        [[nodiscard]] inline Vector<Type, Dimensions>
        rotate(Type angleDegrees) const noexcept
                requires(Dimensions == 2) {
            const auto rad = toRadians(angleDegrees);
            const auto cos = static_cast<Type>(std::cos(rad));
            const auto sin = static_cast<Type>(std::sin(rad));

            return {x() * cos - y() * sin,
                    x() * sin + y() * cos};
        }
    };

    using Vector2u = Vector<std::uint32_t, 2>;
    using Vector3f = Vector<float, 3>;

    [[nodiscard]] Vector3f
    rotateV3f(Vector3f input, float angle, Vector3f axis) noexcept;


} // namespace math
