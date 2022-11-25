/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

namespace base {

    template<typename Callback>
    struct ScopeGuard {
        constexpr
        ScopeGuard(Callback &&callback) noexcept
                : m_callback(std::forward<Callback>(callback)) {
        }

        constexpr
        ~ScopeGuard() noexcept {
            m_callback();
        }

    private:
        Callback m_callback;
    };
    
} // namespace base
