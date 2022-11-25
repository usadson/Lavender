/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
*/

#pragma once

#include <cstdint>

namespace network {

    struct Port {
        [[nodiscard]] constexpr explicit
        Port(std::uint16_t value) noexcept
            : m_value(value) {
        }

        [[nodiscard]] constexpr std::uint16_t
        value() const noexcept {
            return m_value;
        }

    private:
        std::uint16_t m_value;
    };

} // namespace network
