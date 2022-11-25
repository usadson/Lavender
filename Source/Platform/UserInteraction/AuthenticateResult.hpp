/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <string>

namespace platform::user_interaction {

    struct AuthenticateResult {
        enum class Status {
#define PLATFORM_USER_INTERACTION_AUTHENTICATE_RESULT_ITERATE_STATUSES(REGISTER_STATUS) \
            REGISTER_STATUS(FAILED_GENERIC, 0x1000000) \
            REGISTER_STATUS(FAILED_NO_AUTHENTICATION_METHOD_SET_UP, 0x1000001) \

#define REGISTER_STATUS(enumeration, ordinal) enumeration = ordinal,
            PLATFORM_USER_INTERACTION_AUTHENTICATE_RESULT_ITERATE_STATUSES(REGISTER_STATUS)
#undef REGISTER_STATUS
        };

        [[nodiscard]] constexpr explicit
        AuthenticateResult(Status status, std::string &&message = "") noexcept
                : m_status(status)
                , m_message(std::move(message)) {
        }

        [[nodiscard]] constexpr Status
        status() const noexcept {
            return m_status;
        }

        [[nodiscard]] constexpr const std::string &
        message() const noexcept {
            return m_message;
        }

    private:
        Status m_status;
        std::string m_message;
    };

} // namespace platform::user_interaction

[[nodiscard]] constexpr std::string_view
toString(platform::user_interaction::AuthenticateResult::Status status) noexcept {
    switch (status) {
#define CHECK_STATUS(enumeration, ordinal) \
        case platform::user_interaction::AuthenticateResult::Status::enumeration:                                          \
        return #enumeration;

        PLATFORM_USER_INTERACTION_AUTHENTICATE_RESULT_ITERATE_STATUSES(CHECK_STATUS)
#undef CHECK_STATUS
    }

    return "(invalid)";
}
