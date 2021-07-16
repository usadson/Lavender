/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 *
 * Use these types instead of including GLEW when you can, to reduce compile
 * time.
 */

#pragma once

namespace GL {

    using UnsignedIntType = unsigned int;
    using IntType = int;

    using EnumType = UnsignedIntType;

    using TextureType = UnsignedIntType;
    using FramebufferType = TextureType;
    using SizeI = int;

} // namespace GL
