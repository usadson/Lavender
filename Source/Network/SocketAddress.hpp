/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
*/

#pragma once

#include "Source/Network/IPv4Address.hpp"
#include "Source/Network/Port.hpp"

namespace network {

    struct SocketAddress {
        [[nodiscard]] constexpr
        SocketAddress(IPv4Address address, Port port) noexcept
                : m_address(address), m_port(port) {
        }

        [[nodiscard]] constexpr IPv4Address
        address() const noexcept {
            return m_address;
        }

        [[nodiscard]] constexpr Port
        port() const noexcept {
            return m_port;
        }

    private:
        IPv4Address m_address;
        Port m_port;
    };

} // namespace network
