/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <GL/glew.h>

#include "Source/Resources/GraphicsHandleBase.hpp"

namespace gle {

    class GraphicsHandle
            : public resources::GraphicsHandleBase {
        GLuint m_vao{};
        GLuint m_vbo{};

    public:
        [[nodiscard]] inline constexpr
        GraphicsHandle(GLuint vao, GLuint vbo) noexcept
                : m_vao(vao)
                , m_vbo(vbo) {
        }

        [[nodiscard]] inline constexpr GLuint
        vao() const noexcept {
            return m_vao;
        }

        [[nodiscard]] inline constexpr GLuint
        vbo() const noexcept {
            return m_vbo;
        }
    };

} // namespace gle
