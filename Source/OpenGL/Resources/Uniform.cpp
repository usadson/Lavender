/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Source/OpenGL/Resources/Uniform.hpp"

#include <GL/glew.h>

namespace gle {

    void
    UniformMatrix4::store(const math::Matrix4x4<float> &matrix) noexcept {
        glUniformMatrix4fv(location(), 1, GL_TRUE, &matrix.m_data[0][0]);
    }

} // namespace gle
