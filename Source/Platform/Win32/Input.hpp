/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <optional>

#include <cstdint>

#include "Source/Input/KeyboardKey.hpp"

namespace platform::win32 {

    class Input final {
    public:
        Input() = delete;
        ~Input() = delete;

        [[nodiscard]] static std::optional<input::KeyboardKey> 
        translateKey(std::uintmax_t) noexcept;
    };

} // namespace platform::win32
