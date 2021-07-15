/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * An input controller is a game controller, and not specifically just a
 * gamepad or joystick.
 */

#pragma once

#include "Source/Input/KeyboardAction.hpp"
#include "Source/Input/KeyboardKey.hpp"

namespace input {

    enum class MouseButton {
        /**
         * NONE is used when the update is a move update
         */
        NONE,

        LEFT,
        MIDDLE,
        RIGHT
    };

    struct MouseUpdate {
        MouseButton button;
        bool isPressed;

        float moveX{};
        float moveY{};
    };

} // namespace input