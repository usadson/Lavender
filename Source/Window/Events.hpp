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
#include "Source/Window/Options.hpp"

class WindowAPI;

namespace window {

    struct WindowEvent 
            : public event::Event {
        [[nodiscard]] inline constexpr explicit 
        WindowEvent(WindowAPI *window) noexcept 
                : m_window(window) { 
        }

        [[nodiscard]] inline constexpr WindowAPI *
        window() noexcept { 
            return m_window;
        }
        
    private:
        WindowAPI *m_window;
    };

    //
    // Fired when the system, user or application requests the window to be 
    // closed.
    //
    // Cancelling this event means the window ignores the request.
    //
    struct CloseRequestedEvent final
        : public WindowEvent
        , public event::Cancellable {
        enum class Reason {
            UNKNOWN = 0,
            APPLICATION = 1,
            USER_PRESSED_CLOSE_BUTTON = 2,
            SHORTCUT_ALT_F4_PRESSED = 3,
        };

        [[nodiscard]] inline constexpr
        CloseRequestedEvent(WindowAPI *window, Reason reason) noexcept
                : WindowEvent(window), m_reason(reason) {
        }

        [[nodiscard]] inline constexpr Reason
        reason() const noexcept {
            return m_reason;
        }
        
    private:
        Reason m_reason;
    };

    struct FocusGainedEvent final
            : public WindowEvent {
        [[nodiscard]] inline constexpr explicit 
        FocusGainedEvent(WindowAPI *window) noexcept
                : WindowEvent(window) { 
        }
    };

    struct FocusLostEvent final
            : public WindowEvent {
        [[nodiscard]] inline constexpr explicit 
        FocusLostEvent(WindowAPI *window) noexcept 
                : WindowEvent(window) { 
        }
    };

    struct VisibilityChangedEvent final
            : public WindowEvent {
        [[nodiscard]] inline constexpr 
        VisibilityChangedEvent(WindowAPI *window, WindowVisibilityOption from, 
            WindowVisibilityOption to) noexcept 
                : WindowEvent(window), m_from(from), m_to(to) { 
        }

        [[nodiscard]] inline constexpr WindowVisibilityOption
        from() const noexcept { 
            return m_from;
        }

        [[nodiscard]] inline constexpr WindowVisibilityOption
        to() const noexcept { 
            return m_to;
        }

    private:
        WindowVisibilityOption m_from;
        WindowVisibilityOption m_to;
    };

} // namespace window

[[nodiscard]] inline constexpr std::string_view 
toString(window::CloseRequestedEvent::Reason reason) noexcept {
    using Event = window::CloseRequestedEvent;
    using Reason = Event::Reason;

    switch (reason) {
        case Reason::UNKNOWN: return "UNKNOWN";
        case Reason::APPLICATION: return "APPLICATION";
        case Reason::USER_PRESSED_CLOSE_BUTTON: return "USER_PRESSED_CLOSE_BUTTON";
        case Reason::SHORTCUT_ALT_F4_PRESSED: return "SHORTCUT_ALT_F4_PRESSED";
    }

    return "(invalid)";
}
