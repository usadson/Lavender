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
#include <queue>
#include <vector>

#include "Source/Base/Error.hpp"

namespace event {

    template<typename EventType>
    class EventHandlerQueue {
    public:
        using FunctionType = base::Error(EventType &);
        using HandlerType = std::function<FunctionType>;

        [[nodiscard]]
        EventHandlerQueue() noexcept = default;

        EventHandlerQueue(EventHandlerQueue &&) noexcept = delete;
        EventHandlerQueue(const EventHandlerQueue &) noexcept = delete;

        inline void 
        operator+=(HandlerType handler) noexcept {
            m_handlers.push_back(std::forward<HandlerType>(handler));
        }

        void
        invokeLater(EventType &&event) noexcept {
            ++m_firedEventsCount;
            m_queue.push(std::move(event));
        }

        [[nodiscard]] base::Error
        processAll() noexcept {
            while (!m_queue.empty()) {
                auto event = m_queue.front();
                m_queue.pop();
                for (auto& handler : m_handlers) {
                    if (auto error = handler(event))
                        return error;
                }
            }

            return base::Error::success();
        }

        [[nodiscard]] constexpr std::size_t 
        firedEventsCount() const noexcept {
            return m_firedEventsCount;
        }

    private:
        std::vector<HandlerType> m_handlers{};
        std::queue<EventType> m_queue{};
        std::size_t m_firedEventsCount{};
    };

} // namespace event
