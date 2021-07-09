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

#include <GL/glew.h>

#include "Source/OpenGL/DebugMessenger.hpp"
#include "Source/Window/WindowAPI.hpp"

constexpr std::string_view g_vertexShaderCode = R"(
    #version 150 core

    in vec2 position;

    void main() {
        gl_Position = vec4(position, 0.0, 1.0);
    }
)";

constexpr std::string_view g_fragmentShaderCode = R"(
    #version 150 core

    out vec4 outColor;

    void main() {
        outColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
)";

namespace gle {

    resources::GraphicsHandleBase *
    Core::createModel(const resources::ModelGeometry &geometry) noexcept {
        GLuint vao{};
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint vbo{};
        glGenBuffers(1, &vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(std::size(geometry.vertices) * sizeof(geometry.vertices[0])),
                     std::data(geometry.vertices), GL_STATIC_DRAW);

        static_assert(std::is_same_v<GLfloat, float>);
        glVertexAttribPointer(m_shaderAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(m_shaderAttribPosition);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return &m_graphicsHandles.emplace_back(vao, vbo);
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

        const auto location = glGetAttribLocation(m_shaderProgram->programID(), "position");
        if (location == -1) {
            std::printf("[GL] Core: failed to find attribute location named \"position\"\n");
            return false;
        }

        m_shaderAttribPosition = static_cast<GLuint>(location);
        glEnableVertexAttribArray(m_shaderAttribPosition);
        glBindFragDataLocation(m_shaderProgram->programID(), 0, "outColor");

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

        for (const auto &handle : m_graphicsHandles) {
            glBindVertexArray(handle.vao());
            glBindBuffer(GL_ARRAY_BUFFER, handle.vbo());
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
            printf("[GL] Core: Error: %u\n", err);
    }

} // namespace gle
