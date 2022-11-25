/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
*/

#pragma once

#include "Source/Network/Port.hpp"

namespace network::common_ports {

    constexpr const Port http{ 80 };
    constexpr const Port https{ 443 };

    constexpr const Port ssdp{ 1900 };

} // namespace network::common_ports
