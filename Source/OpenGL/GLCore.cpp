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

#include <string_view>
#include <type_traits>

#include <cassert>

#include <GL/glew.h>

#include "Source/OpenGL/DebugMessenger.hpp"
#include "Source/Window/WindowAPI.hpp"

constexpr std::string_view g_vertexShaderCode = R"(
    #version 150 core

    in vec2 position;
    in vec2 vertex_textureCoordinates;

    out vec2 fragment_textureCoordinates;

    void main() {
        gl_Position = vec4(position, 0.0, 1.0);
        fragment_textureCoordinates = vertex_textureCoordinates;
    }
)";

constexpr std::string_view g_fragmentShaderCode = R"(
    #version 150 core

    in vec2 fragment_textureCoordinates;

    out vec4 outColor;

    uniform sampler2D texAlbedo;

    void main() {
        vec4 albedoColor = texture(texAlbedo, fragment_textureCoordinates);
        outColor = albedoColor;
    }
)";

namespace gle {

    resources::ModelGeometryDescriptor *
    Core::createModelGeometry(const resources::ModelGeometry &geometry) noexcept {
        const auto verticesSize = static_cast<GLsizeiptr>(std::size(geometry.vertices) * sizeof(geometry.vertices[0]));
        const auto indicesSize = static_cast<GLsizeiptr>(std::size(geometry.indices) * sizeof(geometry.indices[0]));

        GLuint vao{};
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint vbo{};
        glGenBuffers(1, &vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verticesSize, std::data(geometry.vertices), GL_STATIC_DRAW);

        static_assert(std::is_same_v<GLfloat, float>);
        glVertexAttribPointer(m_shaderAttribPosition, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
        glVertexAttribPointer(m_shaderAttribTextureCoordinates, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void *>(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(m_shaderAttribPosition);
        glEnableVertexAttribArray(m_shaderAttribTextureCoordinates);

        GLuint ebo{};
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, std::data(geometry.indices), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return &m_geometryDescriptors.emplace_back(vao, vbo, ebo);
    }

    resources::TextureDescriptor *
    Core::createTexture(const resources::TextureInput &textureInput) noexcept {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     textureInput.width, textureInput.height,
                     0,
                     translateTextureFormat(textureInput.format),
                     GL_FLOAT,
                     std::data(textureInput.pixels));

        if (textureInput.optGenerateMipmap) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        return &m_textureDescriptors.emplace_back(textureID);
    }

    bool
    Core::initialize(WindowAPI *window) {
        m_windowAPI = window;
        if (!initializeGLEW())
            return false;

        DebugMessenger::setup();

        const auto size = m_windowAPI->queryFramebufferSize();
        glViewport(0, 0, static_cast<GLsizei>(size.x()), static_cast<GLsizei>(size.y()));

        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        m_shaderProgram = std::make_unique<ShaderProgram>(
            Shader(Shader::ConstructionMode::GLSL_SOURCE, ShaderType::VERTEX, g_vertexShaderCode),
            Shader(Shader::ConstructionMode::GLSL_SOURCE, ShaderType::FRAGMENT, g_fragmentShaderCode)
        );

        if (!m_shaderProgram->isValid())
            return false;

        auto location = glGetAttribLocation(m_shaderProgram->programID(), "position");
        if (location == -1) {
            std::printf("[GL] Core: failed to find attribute location named \"position\"\n");
            return false;
        }

        m_shaderAttribPosition = static_cast<GLuint>(location);
        glEnableVertexAttribArray(m_shaderAttribPosition);

        location = glGetAttribLocation(m_shaderProgram->programID(), "vertex_textureCoordinates");
        if (location == -1) {
            std::printf("[GL] Core: failed to find attribute location named \"vertex_textureCoordinates\"\n");
            return false;
        }

        m_shaderAttribTextureCoordinates = static_cast<GLuint>(location);
        glEnableVertexAttribArray(m_shaderAttribTextureCoordinates);

        glBindFragDataLocation(m_shaderProgram->programID(), 0, "outColor");

        glUseProgram(m_shaderProgram->programID());
        auto uniformLocation = glGetUniformLocation(m_shaderProgram->programID(), "texAlbedo");
        glUniform1i(uniformLocation, 0); // texture bank 0

        return true;
    }

    bool
    Core::initializeGLEW() noexcept {
        auto status = glewInit();

        if (status != GLEW_OK) {
            std::printf("[GL] Core: failed to initialize GLEW: %s\n", glewGetErrorString(status));
            return false;
        }

        return true;
    }

    void
    Core::renderEntities() noexcept {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(m_shaderProgram->programID());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureDescriptors.back().textureID());

        for (const auto &handle : m_geometryDescriptors) {
            glBindVertexArray(handle.vao());
            glBindBuffer(GL_ARRAY_BUFFER, handle.vbo());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle.ebo());
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }

//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindVertexArray(0);
//
//        glUseProgram(0);

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
            printf("[GL] Core: Error: %u\n", err);
    }

    GLenum
    Core::translateTextureFormat(resources::TextureFormat textureFormat) noexcept {
        switch (textureFormat) {
            case resources::TextureFormat::RGB:
                return GL_RGB;
            case resources::TextureFormat::RGBA:
                return GL_RGBA;
#ifndef __clang__
            default:
                assert(false);
                return GL_FALSE;
#endif
        }
    }

} // namespace gle
