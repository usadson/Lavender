/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <GL/glew.h>

#include "Source/Resources/SkinDescriptor.hpp"

namespace gle {

    class SkinDescriptor final
            : public resources::SkinDescriptor {
        GLuint m_jointVBO;
        GLuint m_weightVBO;

    public:
        [[nodiscard]] inline constexpr
        SkinDescriptor(GLuint jointVBO, GLuint weightVBO) noexcept
                : m_jointVBO(jointVBO)
                , m_weightVBO(weightVBO) {
        }

        [[nodiscard]] inline constexpr GLuint
        jointVBO() const noexcept {
            return m_jointVBO;
        }

        [[nodiscard]] inline constexpr GLuint
        weightVBO() const noexcept {
            return m_weightVBO;
        }
    };

} // namespace gle
