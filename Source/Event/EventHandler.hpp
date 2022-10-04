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
#include <vector>

#include "Source/Base/Error.hpp"

namespace event {

    template<typename EventType>
    class EventHandler {
    public:
        using FunctionType = base::Error(EventType &);
        using HandlerType = std::function<FunctionType>;

        [[nodiscard]] inline
        EventHandler() noexcept = default;

        EventHandler(EventHandler &&) noexcept = delete;
        EventHandler(const EventHandler &) noexcept = delete;

        inline void 
        operator+=(HandlerType handler) noexcept {
            m_handlers.push_back(std::forward<HandlerType>(handler));
        }

        [[nodiscard]] inline base::Error
        invoke(EventType &event) noexcept {
            ++m_firedEventsCount;

            for (auto &handler : m_handlers) {
                if (auto error = handler(event))
                    return error;
            }

            return base::Error::success();
        }

        [[nodiscard]] inline constexpr std::size_t 
        firedEventsCount() const noexcept {
            return m_firedEventsCount;
        }

    private:
        std::vector<HandlerType> m_handlers{};
        std::size_t m_firedEventsCount{};
    };

} // namespace event
