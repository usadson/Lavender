/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#include "ShaderProgram.hpp"

#include <array>
#include <iostream>

#include <cassert>
#include <cstdio>

#include <GL/glew.h>

#include "Source/OpenGL/Shaders/Shader.hpp"
#include "Source/OpenGL/Utils.hpp"


namespace gle {

    ShaderProgram::ShaderProgram(Shader &&inVertexShader, Shader &&inFragmentShader) noexcept
            : m_programID(glCreateProgram())
            , m_vertexShader(std::move(inVertexShader))
            , m_fragmentShader(std::move(inFragmentShader)) {
        if (m_programID == 0)
            return;

        m_vertexShader.attach(m_programID);
        m_fragmentShader.attach(m_programID);
        glLinkProgram(m_programID);

        GLint success{};
        glGetProgramiv(m_programID, GL_LINK_STATUS, &success);

        if (success == GL_FALSE) {
            std::array<char, 512> buffer{};
            glGetProgramInfoLog(m_programID, std::size(buffer), nullptr, std::data(buffer));
            std::cerr << "[GL] ShaderProgram: link error\n";
            std::cerr << "[GL] ShaderProgram: " << std::data(buffer) << '\n';
            return;
        }

        glUseProgram(m_programID);
    }

    ShaderProgram::~ShaderProgram() noexcept {
        m_vertexShader.detach();
        m_fragmentShader.detach();

        if (m_programID != 0) {
            glDeleteProgram(m_programID);
        }
    }

    void
    ShaderProgram::printUniforms() const noexcept {
        GLint count;
        glGetProgramiv(m_programID, GL_ACTIVE_UNIFORMS, &count);
        std::printf("[GL] Shader: printUniforms: Active Uniforms: %d\n", count);

        assert(count >= 0);

        std::array<GLchar, 16> name{};
        GLsizei nameLength;
        GLint size;
        GLenum type;
        for (GLuint i = 0; i < static_cast<GLuint>(count); i++) {
            glGetActiveUniform(m_programID, i, std::size(name), &nameLength, &size, &type, std::data(name));

            static_cast<void>(nameLength);
            std::printf("[GL] Shader: printUniforms: #%d Type: %s Name: %s\n",
                        i, utils::convertGLTypeToString(type).data(), std::data(name));
        }
    }

} // namespace gle
