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

namespace input::bluetooth {

    class BluetoothManager {
    public:
        virtual inline
        ~BluetoothManager() noexcept = default;

        [[nodiscard]] virtual base::Error 
        initialize() noexcept = 0;
    };

} // namespace input::bluetooth