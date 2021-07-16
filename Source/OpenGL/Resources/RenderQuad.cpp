/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#include "Source/OpenGL/Resources/RenderQuad.hpp"

#include <array>

#include <cassert>

#include <GL/glew.h>

namespace gle {

    bool
    RenderQuad::generate() noexcept {
        constexpr const std::array<GLfloat, 8> vertexData{
            -1.0f,  1.0f,
            -1.0f, -1.0f,
            1.0f,  1.0f,
            1.0f, -1.0f,
        };

        // setup plane VAO
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        if (m_vao == 0 || m_vbo == 0) {
            return false;
        }

        glBufferData(GL_ARRAY_BUFFER, std::size(vertexData) * sizeof(GLfloat), std::data(vertexData), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return glGetError() == GL_NO_ERROR;
    }

    RenderQuad::~RenderQuad() noexcept {
        if (m_vbo != 0)
            glDeleteBuffers(1, &m_vbo);

        if (m_vao != 0)
            glDeleteVertexArrays(1, &m_vao);
    }

    void
    RenderQuad::draw() const noexcept {
        assert(m_vao != 0);
        assert(m_vbo != 0);
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

} // namespace gle
