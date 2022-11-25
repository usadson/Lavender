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

namespace network::common_addresses {

    constexpr IPv4Address loopback{ 127, 0, 0, 1 };

    constexpr IPv4Address ssdpMulticastChannel{239, 255, 255, 250 };

} // namespace network::common_addresses
