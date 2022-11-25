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

#include "Source/Base/ErrorOr.hpp"
#include "Source/Event/EventHandler.hpp"

namespace platform {

    namespace user_interaction {
        struct AuthenticateEvent;
        struct AuthenticateRequest;
    } // namespace user_interaction

    class Platform {
    public:
        [[nodiscard]] static base::ErrorOr<std::unique_ptr<Platform>>
        create() noexcept;

        virtual
        ~Platform() noexcept = default;

        event::EventHandler<user_interaction::AuthenticateEvent> postAuthenticate;

        virtual void
        authenticate(user_interaction::AuthenticateRequest &&) noexcept = 0;
    };

} // namespace platform
