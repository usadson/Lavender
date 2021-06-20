/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <array>

#include <cstdint>

namespace math {

    template <typename Type, std::size_t Dimensions>
        requires(Dimensions >= 2)
    class Vector {
        std::array<Type, Dimensions> m_data{};

    public:
        [[nodiscard]] Vector() = default;
        [[nodiscard]] Vector(Vector &&) = default;
        [[nodiscard]] Vector(const Vector &) = default;
        [[nodiscard]] Vector &operator=(Vector &&) = default;
        [[nodiscard]] Vector &operator=(const Vector &) = default;

        template <typename... CType>
        [[nodiscard]] Vector(CType... values) noexcept
            : m_data{(values)...} {
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
    };

    using Vector2u = Vector<std::uint32_t, 2>;

} // namespace math
