/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <trisan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <string_view>

namespace input {

#define LAVENDER_INPUT_ITERATE_STANDARD_CURSORS(REGISTER_CURSOR) \
        REGISTER_CURSOR(ARROW) \
        REGISTER_CURSOR(ARROW_WITH_HOURGLASS) \
        REGISTER_CURSOR(HAND) \
        REGISTER_CURSOR(HELP) \
        REGISTER_CURSOR(HOURGLASS) \
        REGISTER_CURSOR(I_BEAM) \


    enum class StandardCursor {
#define LAVENDER_INPUT_REGISTER_STANDARD_CURSOR_ENUMERATION(enumeration) enumeration,
        LAVENDER_INPUT_ITERATE_STANDARD_CURSORS(LAVENDER_INPUT_REGISTER_STANDARD_CURSOR_ENUMERATION)
#undef LAVENDER_INPUT_REGISTER_STANDARD_CURSOR_ENUMERATION
    };

} // namespace input

#ifndef LAVENDER_NO_TO_STRING
[[nodiscard]] constexpr std::string_view
toString(input::StandardCursor standardCursor) noexcept {
    switch (standardCursor) {
#define LAVENDER_INPUT_CHECK_STANDARD_CURSOR(enumeration) \
        case input::StandardCursor::enumeration: \
            return #enumeration;

        LAVENDER_INPUT_ITERATE_STANDARD_CURSORS(LAVENDER_INPUT_CHECK_STANDARD_CURSOR)
#undef LAVENDER_INPUT_CHECK_STANDARD_CURSOR
    }

    return "(invalid)";
}
#endif // !LAVENDER_NO_TO_STRING
