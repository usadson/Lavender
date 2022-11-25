/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <functional>
#include <mutex>
#include <queue>

#include "Source/Base/Error.hpp"

namespace event {

    class FunctionQueue {
    public:
        [[nodiscard]]
        FunctionQueue() noexcept;
        ~FunctionQueue() noexcept;

        void
        post(std::function<base::Error ()> &&) noexcept;

        [[nodiscard]] base::Error
        processAll() noexcept;

    private:
        std::mutex m_mutex;
        std::queue<std::function<base::Error()>> m_functions;
    };

} // namespace event
