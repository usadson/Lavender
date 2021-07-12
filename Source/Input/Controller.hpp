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

    struct Controller {
        bool moveLeft{};
        bool moveRight{};
        bool moveForward{};
        bool moveBackward{};
        bool moveUp{};
        bool moveDown{};
    };

} // namespace input