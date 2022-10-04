/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
* All Rights Reserved.
 */

#include "OpenGLHelpers.hpp"

#include <GL/glew.h>

namespace gle {

    utils::Version<int>
    openGLVersion() {
        int major{0};
        glGetIntegerv(GL_MAJOR_VERSION, &major);

        int minor{0};
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        return utils::Version<int>{major, minor, 0};
    }

} // namespace gle
