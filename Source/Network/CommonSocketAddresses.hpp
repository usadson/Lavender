/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
*/

#pragma once

#include "Source/Network/CommonAddresses.hpp"
#include "Source/Network/CommonPorts.hpp"
#include "Source/Network/SocketAddress.hpp"

namespace network::common_socket_address {

    constexpr const SocketAddress ssdpMulticast{
        common_addresses::ssdpMulticastChannel,
        common_ports::ssdp
    };

} // namespace network::common_socket_address
