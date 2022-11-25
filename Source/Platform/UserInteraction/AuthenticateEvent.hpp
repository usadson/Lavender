/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include "Source/Event/Event.hpp"

#include "Source/Platform/UserInteraction/AuthenticateRequest.hpp"
#include "Source/Platform/UserInteraction/AuthenticateResult.hpp"

namespace platform::user_interaction {

    struct AuthenticateEvent
            : public event::Event {
        [[nodiscard]] inline
        AuthenticateEvent(AuthenticateRequest &&request, AuthenticateResult &&result) noexcept
                : m_request(std::move(request))
                , m_result(std::move(result)) {
        }

        [[nodiscard]] constexpr AuthenticateRequest &
        request() noexcept {
            return m_request;
        }

        [[nodiscard]] constexpr const AuthenticateRequest &
        request() const noexcept {
            return m_request;
        }

    private:
        AuthenticateRequest m_request;
        AuthenticateResult m_result;
    };

} // namespace platform::user_interaction
