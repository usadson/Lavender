/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

namespace event {

    enum class CancelStatus {
        NOT_CANCELLED = 0,
        CANCEL_BUT_CONTINUE_EVENT_HANDLING = 1,
        CANCEL_IMMEDIATELY = 2,
    };

    struct Cancellable {
        [[nodiscard]] inline constexpr CancelStatus
        cancelStatus() const noexcept {
            return m_status;
        }

        inline constexpr void 
        setCancelled(CancelStatus status) noexcept {
            m_status = status;
        }
        
    private:
        CancelStatus m_status{};
    };

} // namespace event
