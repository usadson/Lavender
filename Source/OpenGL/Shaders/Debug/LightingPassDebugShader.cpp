/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#include "Source/OpenGL/Shaders/Debug/LightingPassDebugShader.hpp"

#include <GL/glew.h>

namespace gle {

    bool
    LightingPassDebugShader::setIndex(int index) noexcept {
        if (!m_program) {
            std::puts("[GL] LightingPassDebugShader: setIndex: no program");
            return false;
        }

        const auto program = m_program->programID();
        if (program == 0) {
            std::puts("[GL] LightingPassDebugShader: setIndex: no program ID set");
            return false;
        }

        glUseProgram(program);

        auto location = glGetUniformLocation(program, "u_mode");
        if (location == -1) {
            std::puts("[GL] LightingPassDebugShader: setIndex: failed to find uniform");
            return false;
        }

        glUniform1i(location, index);

        if (glGetError() == GL_NO_ERROR)
            return true;

        std::puts("[GL] LightingPassDebugShader: setIndex: GL error triggered");
        return false;
    }

    bool
    LightingPassDebugShader::setup() noexcept {
        m_program = std::make_unique<ShaderProgram>(
            Shader(Shader::ConstructionMode::GLSL_PATH, ShaderType::VERTEX, "lighting_pass_debug.vert"),
            Shader(Shader::ConstructionMode::GLSL_PATH, ShaderType::FRAGMENT, "lighting_pass_debug.frag")
        );

        if (!m_program->isValid()) {
            std::puts("[GL] LightingPassDebugShader: failed to load shader");
            return false;
        }

        const auto program = m_program->programID();
        glUseProgram(program);
        glUniform1i(glGetUniformLocation(program, "gPosition"), 0);
        glUniform1i(glGetUniformLocation(program, "gNormal"), 1);
        glUniform1i(glGetUniformLocation(program, "gAlbedoSpec"), 2);

        glUniform1i(glGetUniformLocation(program, "gAlbedoSpec"), 2);

        return glGetError() == GL_NO_ERROR;
    }

} // namespace gle
