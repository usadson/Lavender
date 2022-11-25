/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#include "Platform.hpp"

#include "Source/Platform/UserInteraction/AuthenticateEvent.hpp"

namespace platform::win32 {

    void
    Win32Platform::authenticate(user_interaction::AuthenticateRequest &&request) noexcept {
        user_interaction::AuthenticateResult result{
            user_interaction::AuthenticateResult::Status::FAILED_NO_AUTHENTICATION_METHOD_SET_UP
        };

        user_interaction::AuthenticateEvent event{std::move(request), std::move(result)};
        postAuthenticate.invoke(event).displayErrorMessageBox();
    }

} // namespace platform::win32
