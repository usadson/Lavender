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

#include <random>
#include <string_view>
#include <type_traits>

#include <cassert>

#include <GL/glew.h>

#include "Source/ECS/EntityList.hpp"
#include "Source/Interface/Camera.hpp"
#include "Source/OpenGL/DebugMessenger.hpp"
#include "Source/Window/WindowAPI.hpp"

namespace gle {

    static_assert(std::is_same_v<GLfloat, float>);

    resources::ModelGeometryDescriptor *
    Core::createModelGeometry(const resources::ModelGeometry &geometry) noexcept {
        GLuint vao{};
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        auto ebo = createElementBuffer(geometry.indices);
        if (!ebo.has_value())
            return nullptr;

        auto vbo = createVertexBuffer(geometry.vertices);
        if (!vbo.has_value())
            return nullptr;

        auto tbo = createTextureBuffer(geometry.textureCoordinates);
        if (!tbo.has_value())
            return nullptr;

        glEnableVertexAttribArray(m_gBufferShader.attributeLocationPosition());
        glEnableVertexAttribArray(m_gBufferShader.attributeLocationTextureCoordinates());

        glBindVertexArray(0);
        m_geometryDescriptors.push_back(std::make_unique<ModelGeometryDescriptor>(
                vao, vbo.value(), ebo.value(), tbo.value(), 0, static_cast<GLsizei>(std::size(geometry.indices)), GL_UNSIGNED_INT));
        return m_geometryDescriptors.back().get();
    }

    std::optional<unsigned int>
    Core::createElementBuffer(const std::vector<resources::ModelGeometry::IndexType> &indices) const noexcept {
        GLuint ebo{};

        glGenBuffers(1, &ebo);
        if (glGetError() != GL_NO_ERROR)
            return std::nullopt;

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        const auto size = static_cast<GLsizeiptr>(std::size(indices) * sizeof(indices[0]));
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, std::data(indices), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return ebo;
    }

    bool
    Core::createLights() noexcept {
        constexpr const std::size_t lightCount = 32;
        std::default_random_engine engine;
        std::uniform_real_distribution<float> positionDistrib(-3.0f, 3.0f);
        std::uniform_real_distribution<float> colorDistrib(0.5, 1.0);

        for (std::size_t i = 0; i < lightCount; i++) {
            if (!m_lightingPassShader.setLight(i,
                    {positionDistrib(engine), positionDistrib(engine), positionDistrib(engine)},
                    {colorDistrib(engine), colorDistrib(engine), colorDistrib(engine)},
                    0.3f)) {
                std::printf("[GL] Core: failed to set light #%zu\n", i);
                return false;
            }
        }

        return true;
    }

    std::optional<unsigned int>
    Core::createTextureBuffer(const std::vector<resources::ModelGeometry::TextureCoordType> &textureCoordinates) const noexcept {
        GLuint tbo{};

        glGenBuffers(1, &tbo);
        if (glGetError() != GL_NO_ERROR)
            return std::nullopt;

        glBindBuffer(GL_ARRAY_BUFFER, tbo);

        const auto size = static_cast<GLsizeiptr>(std::size(textureCoordinates) * sizeof(textureCoordinates[0]));
        glBufferData(GL_ARRAY_BUFFER, size, std::data(textureCoordinates), GL_STATIC_DRAW);

        glVertexAttribPointer(m_gBufferShader.attributeLocationTextureCoordinates(), 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return tbo;
    }

    std::optional<unsigned int>
    Core::createVertexBuffer(const std::vector<resources::ModelGeometry::VertexType> &vertices) const noexcept {
        GLuint vbo{};

        glGenBuffers(1, &vbo);
        if (glGetError() != GL_NO_ERROR)
            return std::nullopt;

        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        const auto size = static_cast<GLsizeiptr>(std::size(vertices) * sizeof(vertices[0]));
        glBufferData(GL_ARRAY_BUFFER, size, std::data(vertices), GL_STATIC_DRAW);

        glVertexAttribPointer(m_gBufferShader.attributeLocationPosition(), 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return vbo;
    }

    bool
    Core::initialize(WindowAPI *window) {
        m_windowAPI = window;
        if (!initializeGLEW())
            return false;

        const auto contextVersion = m_windowAPI->queryGLContextVersion();

        DebugMessenger::setup();

        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        if (!m_gBufferShader.setup()) {
            std::puts("[GL] Core: failed to setup GBuffer shader");
            return false;
        }

        if (!m_lightingPassShader.setup()) {
            std::puts("[GL] Core: failed to setup lighting pass shader");
            return false;
        }

        if (!createLights()) {
            std::puts("[GL] Core: failed to create lights");
            return false;
        }

        if (!m_renderQuad.generate()) {
            std::puts("[GL] Core: failed to generate the render quad");
            return false;
        }

        const auto size = m_windowAPI->queryFramebufferSize();
        onResize({size.x(), size.y()});

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glFrontFace(GL_CW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);

        if (contextVersion.major >= 3) {
            glEnable(GL_FRAMEBUFFER_SRGB);
        }

        if (glGetError() != GL_NO_ERROR) {
            std::puts("[GL] Core: failed to do finalizing steps");
            return false;
        }

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
    Core::onResize(math::Size2D<std::uint32_t> size) noexcept {
        glViewport(0, 0, static_cast<GLsizei>(size.width()), static_cast<GLsizei>(size.height()));

        if (!m_gBuffer.generate(size.width(), size.height())) {
            std::puts("[GL] Core: failed to generate GBuffer");
            std::abort();
        }

        glUseProgram(m_gBufferShader.programID());
        m_gBufferShader.uploadProjectionMatrix(math::createPerspectiveProjectionMatrix(
            70, static_cast<float>(size.width()), static_cast<float>(size.height()), 0.1f, 1000));
    }

    void
    Core::renderEntities() noexcept {
        glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer.buffer());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(m_gBufferShader.programID());

        m_gBufferShader.uploadViewMatrix(m_camera->viewMatrix());

        for (const auto &entity : m_entityList->data()) {
            assert(entity != nullptr);

            if (entity->modelDescriptor() == nullptr)
                continue;

            if (entity->modelDescriptor()->albedoTextureDescriptor() != nullptr) {
                auto *albedoTexture = static_cast<const TextureDescriptor *>(entity->modelDescriptor()->albedoTextureDescriptor());

                if (albedoTexture->textureID() != 0) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, albedoTexture->textureID());
                }
            }

            m_gBufferShader.uploadTransformationMatrix(entity->transformation().toMatrix());

            const auto *geometry = static_cast<const ModelGeometryDescriptor *>(entity->modelDescriptor()->geometryDescriptor());
            assert(geometry != nullptr);
            glBindVertexArray(geometry->vao());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->ebo());
            glDrawElements(GL_TRIANGLES, geometry->indexCount(), geometry->indexType(), nullptr);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindVertexArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_gBuffer.texturePosition());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_gBuffer.textureNormal());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_gBuffer.textureColorSpec());

        glUseProgram(m_lightingPassShader.programID());
        m_renderQuad.draw();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
            printf("[GL] Core: Error: %u\n", err);
    }

    const resources::ModelDescriptor *
    Core::uploadModelDescriptor(resources::ModelDescriptor &&modelDescriptor) noexcept {
        m_modelDescriptors.push_back(std::make_unique<resources::ModelDescriptor>(std::forward<resources::ModelDescriptor>(modelDescriptor)));
        return m_modelDescriptors.back().get();
    }

} // namespace gle
