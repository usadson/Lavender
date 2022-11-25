/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
*/

#pragma once

#include <array>

namespace network {

    struct IPv4Address {
        [[nodiscard]] constexpr explicit
        IPv4Address(const std::array<unsigned char, 4> &data) noexcept
                : m_data{ .bytes = data } {
        }

        [[nodiscard]] constexpr explicit
        IPv4Address(unsigned long asLong) noexcept
                : m_data{ .asLong = asLong } {
        }

        [[nodiscard]] constexpr explicit
        IPv4Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d) noexcept
                : m_data{ {a, b, c, d} } {
        }

        [[nodiscard]] constexpr unsigned long
        asLong() const noexcept {
            return m_data.asLong;
        }

        [[nodiscard]] constexpr unsigned char
        operator[](std::size_t index) const noexcept {
            return m_data.bytes[index];
        }

    private:
        union {
            std::array<unsigned char, 4> bytes;
            unsigned long asLong;
        } m_data;
    };

} // namespace network
