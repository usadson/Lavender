/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
*/

#pragma once

#include <memory>

#include "Source/Base/Error.hpp"

namespace devices {

    namespace cast { class CastManager; }

    class DeviceManager {
    public:
        [[nodiscard]]
        DeviceManager() noexcept;

        ~DeviceManager() noexcept;
        
        [[nodiscard]] base::Error
        initialize() noexcept;

    private:
        std::unique_ptr<cast::CastManager> m_castManager;
    };
    
} // namespace devices
