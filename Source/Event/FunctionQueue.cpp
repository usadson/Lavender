/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#include "FunctionQueue.hpp"

namespace event {

    FunctionQueue::FunctionQueue() noexcept = default;
    FunctionQueue::~FunctionQueue() noexcept = default;

    void
    FunctionQueue::post(std::function<base::Error()> &&function) noexcept {
        std::lock_guard guard{ m_mutex };
        m_functions.push(std::move(function));
    }

    base::Error
    FunctionQueue::processAll() noexcept {
        std::queue<std::function<base::Error()>> queue;
        {
            std::lock_guard guard{ m_mutex };
            m_functions.swap(queue);
        }

        while (!queue.empty()) {
            if (const auto error = queue.front()())
                return error;

            queue.pop();
        }

        return base::Error::success();
    }


} // namespace event
