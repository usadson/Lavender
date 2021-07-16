/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 - Tristan Gerritsen <tristan-legal@outlook.com>
 * All Rights Reserved.
 */

#pragma once

#include <string_view>

#include "Source/OpenGL/Types.hpp"

namespace gle::utils {

    [[nodiscard]] std::string_view
    convertGLTypeToString(GL::EnumType) noexcept;

} // namespace gle::utils