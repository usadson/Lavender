/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#define LAVENDER_GLE_SHADER_IMPLEMENTATION

#include "Source/OpenGL/Shaders/AnimatedGBufferShader.hpp"

#include <GL/glew.h>

namespace gle {

    bool
    AnimatedGBufferShader::resolveAttributeLocation(const char *name, GL::UnsignedIntType *destination) const noexcept {
        const auto location = glGetAttribLocation(m_program->programID(), name);

        if (location == -1) {
            std::printf("[GL] GBufferShader: failed to find attribute location named \"%s\"\n", name);
            return false;
        }

        *destination = static_cast<GLuint>(location);
        return true;
    }

    bool
    AnimatedGBufferShader::setup() noexcept {
        m_program = std::make_unique<ShaderProgram>(
            Shader(Shader::ConstructionMode::GLSL_PATH, ShaderType::VERTEX, "anim_gbuffer.vert"),
            Shader(Shader::ConstructionMode::GLSL_PATH, ShaderType::FRAGMENT, "gbuffer.frag")
        );

        if (!m_program->isValid())
            return false;

        if (!resolveAttributeLocation("position", &m_attribPosition)
                || !resolveAttributeLocation("vertex_normal", &m_attribNormal)
                || !resolveAttributeLocation("vertex_textureCoordinates", &m_attribTextureCoordinates)
                || !resolveAttributeLocation("vertex_joint", &m_attribJoint)
                || !resolveAttributeLocation("vertex_joint", &m_attribWeight))
            return false;

        glUseProgram(m_program->programID());

        auto uniformLocation = glGetUniformLocation(m_program->programID(), "texAlbedo");
        glUniform1i(uniformLocation, 0); // texture bank 0

        m_uniformProjection = UniformMatrix4(glGetUniformLocation(m_program->programID(), "u_projection"));
        m_uniformTransformation = UniformMatrix4(glGetUniformLocation(m_program->programID(), "u_transform"));
        m_uniformView = UniformMatrix4(glGetUniformLocation(m_program->programID(), "u_view"));
        m_uniformJointMatrix = UniformMatrix4fArray<12>(glGetUniformLocation(m_program->programID(), "u_joinMatrix"));

        return glGetError() == GL_NO_ERROR;
    }

} // namespace gle
