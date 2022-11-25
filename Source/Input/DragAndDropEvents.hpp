/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include "Source/Event/Cancellable.hpp"
#include "Source/Event/Event.hpp"
#include "Source/Input/DragObject.hpp"
#include "Source/Math/Vector.hpp"

namespace input {

    struct DragAndDropEvent
            : public event::Event
            , public event::Cancellable {
        
        [[nodiscard]] inline constexpr
        DragAndDropEvent(DragObject &&data, math::Vector2f point)
                : m_data(std::move(data))
                , m_point(point) {
        }

        [[nodiscard]] inline constexpr DragObject &
        data() noexcept {
            return m_data;
        }

        [[nodiscard]] inline constexpr const DragObject &
        data() const noexcept {
            return m_data;
        }

        [[nodiscard]] inline constexpr math::Vector2f
        point() const noexcept {
            return m_point;
        }

        [[nodiscard]] inline constexpr bool
        isDropAllowed() const noexcept {
            return m_allowDrop;
        }

        inline constexpr void
        allowDrop() noexcept {
            m_allowDrop = true;
        }

        inline constexpr void
        rejectDrop() noexcept {
            m_allowDrop = false;
        }

    private:
        bool m_allowDrop{false};
        DragObject m_data;
        math::Vector2f m_point;
    };

    using DragEnterEvent = DragAndDropEvent;
    using DragOverEvent = DragAndDropEvent;
    using DropEvent = DragAndDropEvent;
    using DragLeaveEvent = event::Event;

} // namespace input
