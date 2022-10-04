/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include "Source/Input/Bluetooth/BluetoothManager.hpp"

namespace input::bluetooth {

    class Win32BluetoothManager final 
            : public BluetoothManager {
    public:
        ~Win32BluetoothManager() noexcept override;

        [[nodiscard]] base::Error
        initialize() noexcept override;
    };

} // namespace input::bluetooth