/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#include "Source/OpenGL/Renderer/DeferredRenderer.hpp"

#include <cassert>
#include <cstdio>

#include <GL/glew.h>

#include "Source/ECS/EntityList.hpp"
#include "Source/ECS/Scene.hpp"
#include "Source/Interface/Camera.hpp"
#include "Source/OpenGL/GLCore.hpp"
#include "Source/OpenGL/ModelGeometryDescriptor.hpp"
#include "Source/OpenGL/TextureDescriptor.hpp"

namespace gle {

    AttributeLocations
    DeferredRenderer::attributeLocations() noexcept {
        AttributeLocations locations{};

        locations.position = m_gBufferShader.attributeLocationPosition();
        locations.textureCoordinates = m_gBufferShader.attributeLocationTextureCoordinates();
        locations.normal = m_gBufferShader.attributeLocationNormal();

        return locations;
    }

    void
    DeferredRenderer::drawGBuffer() noexcept {
        glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer.buffer());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(m_gBufferShader.programID());

        m_gBufferShader.uploadViewMatrix(core()->camera()->viewMatrix());

        for (const auto &entity : std::data(core()->scene()->entityList())) {
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
    }

    void
    DeferredRenderer::drawLighting() noexcept {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_gBuffer.texturePosition());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_gBuffer.textureNormal());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_gBuffer.textureColorSpec());

#ifdef LAVENDER_BUILD_DEBUG
        switch (renderMode()) {
            case RenderMode::DEFAULT:
                glUseProgram(m_lightingPassShader.programID());
                break;
            default:
                glUseProgram(m_lightingPassDebugShader.programID());
                assert(m_lightingPassDebugShader.setIndex(static_cast<int>(renderMode())));
                break;
        }
#else
        glUseProgram(m_lightingPassShader.programID());
#endif

        m_renderQuad.draw();
    }

    void
    DeferredRenderer::onResize(math::Size2D<std::uint32_t> size) noexcept {
        if (!m_gBuffer.generate(size.width(), size.height())) {
            std::puts("[GL] Core: failed to generate GBuffer");
            std::abort();
        }

        glUseProgram(m_gBufferShader.programID());
        m_gBufferShader.uploadProjectionMatrix(math::createPerspectiveProjectionMatrix(
            70, static_cast<float>(size.width()), static_cast<float>(size.height()), 0.1f, 1000));
    }

    void
    DeferredRenderer::render() noexcept {
        drawGBuffer();
        drawLighting();
    }

    bool
    DeferredRenderer::setLight(std::size_t index, math::Vector3f position, math::Vector3f color, float radius) noexcept {
        return m_lightingPassShader.setLight(index, position, color, radius);
    }

    bool
    DeferredRenderer::setup() noexcept {
        if (!m_gBufferShader.setup()) {
            std::puts("[GL] DeferredRenderer: failed to setup GBuffer shader");
            return false;
        }

        if (!m_lightingPassShader.setup()) {
            std::puts("[GL] DeferredRenderer: failed to setup lighting pass shader");
            return false;
        }

        if (!m_renderQuad.generate()) {
            std::puts("[GL] DeferredRenderer: failed to generate the render quad");
            return false;
        }

        if (!setupDebugEnvironment()) {
            std::puts("[GL] DeferredRenderer: failed to setup debug environment");
            return false;
        }

        return true;
    }

#ifdef LAVENDER_BUILD_DEBUG
    bool
    DeferredRenderer::setupDebugEnvironment() noexcept {
        return m_lightingPassDebugShader.setup();
    }
#endif

} // namespace gle
