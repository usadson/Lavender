/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace math {

    template <typename Type>
    struct Size2D {
        Type m_width;
        Type m_height;

        [[nodiscard]] inline constexpr Type
        height() const noexcept {
            return m_height;
        }

        [[nodiscard]] inline constexpr Type
        width() const noexcept {
            return m_width;
        }
    };
} // namespace math
