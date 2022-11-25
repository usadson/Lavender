/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
*/

#pragma once

#include "Source/Base/Error.hpp"
#include "Source/Network/UDP/Socket.hpp"

namespace devices::cast {

    class CastManager {
    public:
        [[nodiscard]]
        CastManager() noexcept;

        ~CastManager() noexcept;

        [[nodiscard]] base::Error
        initialize() noexcept;

    private:
        void
        startUniCastSocket() noexcept;

        network::udp::Socket m_multiCastSocket{};
        network::udp::Socket m_uniCastSocket{};
    };

} // namespace devices::cast
