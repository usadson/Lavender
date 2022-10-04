/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* Copyright (C) 2021 - 2022 Tristan Gerritsen <tristan@thewoosh.org>
* All Rights Reserved.
 */

#pragma once

#include <cstdint>

namespace base {

    struct Version {
        template <typename T>
        [[nodiscard]] inline constexpr
        Version(T major, T minor, T patch) noexcept
                : m_major(static_cast<std::uint8_t>(major))
                , m_minor(static_cast<std::uint8_t>(minor))
                , m_patch(static_cast<std::uint8_t>(patch)) {
        }

        [[nodiscard]] inline constexpr std::uint32_t
        asCombined() const noexcept {
            return (static_cast<std::uint32_t>(m_major) << 16)
                 | (static_cast<std::uint32_t>(m_minor) << 8)
                 | (static_cast<std::uint32_t>(m_patch));
        }

        [[nodiscard]] inline constexpr std::uint8_t
        major() const noexcept {
            return m_major;
        }

        [[nodiscard]] inline constexpr std::uint8_t
        minor() const noexcept {
            return m_minor;
        }

        [[nodiscard]] inline constexpr std::uint8_t
        patch() const noexcept {
            return m_patch;
        }

    private:
        std::uint8_t m_major{};
        std::uint8_t m_minor{};
        std::uint8_t m_patch{};
    };

} // namespace base
