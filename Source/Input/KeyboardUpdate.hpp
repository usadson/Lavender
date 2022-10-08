/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * An input controller is a game controller, and not specifically just a
 * gamepad or joystick.
 */

#pragma once

#include "Source/Event/Event.hpp"
#include "Source/Input/KeyboardAction.hpp"
#include "Source/Input/KeyboardKey.hpp"

namespace input {

    struct KeyboardUpdate {
        KeyboardKey key;
        KeyboardAction action;
    };

    struct KeyboardEvent : public event::Event { };

    struct KeyboardKeyEvent
            : public KeyboardEvent {
        [[nodiscard]] inline constexpr explicit
        KeyboardKeyEvent(KeyboardKey key)
                : m_key(key) {
        }

        [[nodiscard]] inline constexpr KeyboardKey 
        key() const noexcept {
            return m_key;
        }

    private:
        KeyboardKey m_key;
    };

    struct KeyPressedEvent
            : public KeyboardKeyEvent {
        [[nodiscard]] inline constexpr explicit 
        KeyPressedEvent(KeyboardKey key)
                : KeyboardKeyEvent(key) {
        }
    };

    struct KeyReleasedEvent 
            : public KeyboardKeyEvent {
        [[nodiscard]] inline constexpr explicit 
        KeyReleasedEvent(KeyboardKey key)
                : KeyboardKeyEvent(key) { 
        }
    };

} // namespace input