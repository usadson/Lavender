/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * OpenGL® and the oval logo are trademarks or registered trademarks of
 * Hewlett Packard Enterprise in the United States and/or other
 * countries worldwide.
 */

#include "Source/OpenGL/GLCore.hpp"

#include <GL/glew.h>

namespace gle {

    bool
    Core::initialize(WindowAPI *) {
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        return true;
    }

} // namespace gle
