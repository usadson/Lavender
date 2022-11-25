/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include "Source/Platform/Platform.hpp"

namespace platform::win32 {

    class Win32Platform final
        : public Platform {
    public:
        void
        authenticate(user_interaction::AuthenticateRequest &&) noexcept;
    };

} // namespace platform::win32
