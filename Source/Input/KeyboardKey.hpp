/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * An input controller is a game controller, and not specifically just a
 * gamepad or joystick.
 */

#pragma once

namespace input {

    enum class KeyboardKey {
        INVALID_KEY,

        W,
        A,
        S,
        D,

        SPACE,

        LEFT_SHIFT,
        LEFT_CONTROL,

        NUMPAD0, NUMPAD1, NUMPAD2,
        NUMPAD3, NUMPAD4, NUMPAD5,
        NUMPAD6, NUMPAD7, NUMPAD8,
        NUMPAD9,
    };

} // namespace input