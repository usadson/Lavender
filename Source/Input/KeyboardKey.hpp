/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * An input controller is a game controller, and not specifically just a
 * gamepad or joystick.
 */

#pragma once

#ifdef DELETE
// Warning: certain macros are deleted that clash with enums in input::KeyboardKey
#undef DELETE
#endif

namespace input {

    enum class KeyboardKey {
        INVALID_KEY,

        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        LEFT_SHIFT,
        RIGHT_SHIFT,
        LEFT_CONTROL,
        RIGHT_CONTROL,
        LEFT_ALT,
        RIGHT_ALT,

        SPACE,
        COMMA,                  /*  ,  */
        FULL_STOP,              /*  .  */
        SEMICOLON,              /*  ;  */
        APOSTROPHE,             /*  '  */
        SOLIDUS,                /*  /  */
        REVERSE_SOLIDUS,        /*  \  */
        LEFT_SQUARE_BRACKET,    /*  [  */
        RIGHT_SQUARE_BRACKET,   /*  ]  */
        HYPHEN_MINUS,           /*  -  */
        //LOW_LINE,               /*  _  */

        ESCAPE,
        TAB,
        CAPS_LOCK,
        ENTER,
        BACKSPACE,
        PAGE_UP,
        PAGE_DOWN,
        END,
        HOME,
        SELECT,
        PRINT,
        PRINT_SCREEN,
        INSERT,
        DELETE,
        HELP,

        LEFT_META,
        RIGHT_META,
        LEFT_SUPER = LEFT_META,
        LEFT_LOGO = LEFT_META,
        LEFT_WIN = LEFT_META,
        LEFT_COMMAND = LEFT_META,
        RIGHT_SUPER = RIGHT_META,
        RIGHT_LOGO = RIGHT_META,
        RIGHT_WIN = RIGHT_META,
        RIGHT_COMMAND = RIGHT_META,

        DIGIT0, DIGIT1, DIGIT2, DIGIT3, DIGIT4, DIGIT5, DIGIT6, DIGIT7, DIGIT8, DIGIT9,
        F1, F2, F3, F4, F5, F6, F7, F8, F9,
        F10, F11, F12, F13, F14, F15, F16, F17, F18, F19,
        F20, F21, F22, F23, F24, F25,

        ARROW_LEFT, 
        ARROW_RIGHT,
        ARROW_UP,
        ARROW_DOWN,

        NUMPAD_LOCK,
        NUMPAD_PLUS,
        NUMPAD_MINUS,
        NUMPAD_DIVIDE,
        NUMPAD_MULTIPLY,
        NUMPAD_DECIMAL,
        NUMPAD_ENTER,
        NUMPAD0, NUMPAD1, NUMPAD2,
        NUMPAD3, NUMPAD4, NUMPAD5,
        NUMPAD6, NUMPAD7, NUMPAD8,
        NUMPAD9,
    };

} // namespace input