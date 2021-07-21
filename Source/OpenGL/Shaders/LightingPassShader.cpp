/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#include "Source/OpenGL/Shaders/LightingPassShader.hpp"

namespace gle {

    bool
    LightingPassShader::setLight(std::size_t index, math::Vector3f position, math::Vector3f color) noexcept {
        if (!m_program || index >= 32)
            return false;

        const auto program = m_program->programID();
        if (program == 0)
            return false;

        glUseProgram(program);

        auto location = glGetUniformLocation(program, ("lights[" + std::to_string(index) + "].m_position").c_str());
        glUniform3f(location, position.x(), position.y(), position.z());

        location = glGetUniformLocation(program, ("lights[" + std::to_string(index) + "].m_color").c_str());
        glUniform3f(location, color.x(), color.y(), color.z());

        return glGetError() == GL_NO_ERROR;
    }

    bool
    LightingPassShader::setup() noexcept {
        m_program = std::make_unique<ShaderProgram>(
            Shader(Shader::ConstructionMode::GLSL_PATH, ShaderType::VERTEX, "lighting_pass.vert"),
            Shader(Shader::ConstructionMode::GLSL_PATH, ShaderType::FRAGMENT, "lighting_pass.frag")
        );

        if (!m_program->isValid()) {
            std::puts("[GL] LightingPassShader: failed to load shader");
            return false;
        }

        const auto program = m_program->programID();
        glUseProgram(program);
        glUniform1i(glGetUniformLocation(program, "gPosition"), 0);
        glUniform1i(glGetUniformLocation(program, "gNormal"), 1);
        glUniform1i(glGetUniformLocation(program, "gAlbedoSpec"), 2);

        return glGetError() == GL_NO_ERROR;
    }

} // namespace gle
