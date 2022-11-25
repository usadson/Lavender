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
#include "Source/ECS/PointLight.hpp"
#include "Source/ECS/Scene.hpp"
#include "Source/Interface/Camera.hpp"
#include "Source/OpenGL/GLCore.hpp"
#include "Source/OpenGL/ModelGeometryDescriptor.hpp"
#include "Source/OpenGL/TextureDescriptor.hpp"

namespace gle {

    constexpr base::FunctionErrorGenerator errors{"OpenGLCore", "DeferredRenderer"};

    AttributeLocations
    DeferredRenderer::attributeLocations(CapabilitiesRequired capabilitiesRequired) noexcept {
        AttributeLocations locations{};

        if (capabilitiesRequired.hasSkin) {
            locations.position = m_animatedGBufferShader.attributeLocationPosition();
            locations.textureCoordinates = m_animatedGBufferShader.attributeLocationTextureCoordinates();
            locations.normal = m_animatedGBufferShader.attributeLocationNormal();
            locations.tangent = m_animatedGBufferShader.attributeLocationTangent();
            locations.bitangent = m_animatedGBufferShader.attributeLocationBitangent();
            locations.joint = m_animatedGBufferShader.attributeLocationJoint();
            locations.weight = m_animatedGBufferShader.attributeLocationWeight();
        } else {
            locations.position = m_gBufferShader.attributeLocationPosition();
            locations.textureCoordinates = m_gBufferShader.attributeLocationTextureCoordinates();
            locations.normal = m_gBufferShader.attributeLocationNormal();
            locations.tangent = m_gBufferShader.attributeLocationTangent();
            locations.bitangent = m_gBufferShader.attributeLocationBitangent();
        }

        return locations;
    }

    template<GLint TextureBank>
    static void
    setTexture(GLuint textureID) {
        static GLuint oldValue = 0;
        if (textureID == oldValue)
            return;

        glActiveTexture(TextureBank);
        glBindTexture(GL_TEXTURE_2D, textureID);
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
                setTexture<GL_TEXTURE0>(albedoTexture->textureID());
            } else
                setTexture<GL_TEXTURE0>(0);

            if (entity->modelDescriptor()->normalMapTextureDescriptor() != nullptr) {
                auto *normalMapTexture = static_cast<const TextureDescriptor *>(entity->modelDescriptor()->normalMapTextureDescriptor());
                setTexture<GL_TEXTURE1>(normalMapTexture->textureID());
            } else
                setTexture<GL_TEXTURE1>(0);

            m_gBufferShader.uploadTransformationMatrix(entity->transformation().toMatrix());

            const auto *geometry = static_cast<const ModelGeometryDescriptor *>(entity->modelDescriptor()->geometryDescriptor());
            assert(geometry != nullptr);

            assert(glIsVertexArray(geometry->vao()));

            glBindVertexArray(geometry->vao());
            assert(glGetError() == GL_NO_ERROR);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->ebo());
            

            if (geometry->indexCount() == 0) {
                glDrawArrays(GL_TRIANGLES, 0, geometry->vertexCount());
            } else {
                glDrawElements(GL_TRIANGLES, geometry->indexCount(), geometry->indexType(), nullptr);
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindVertexArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void
    DeferredRenderer::drawLighting() noexcept {
        // I believe glClear isn't necessary, since the GBuffer is filled with colour entirely.
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_gBuffer.texturePosition());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_gBuffer.textureNormal());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_gBuffer.textureColorSpec());

#ifdef LAVENDER_BUILD_DEBUG
        switch (Renderer::renderMode()) {
            case RenderMode::DEFAULT:
                glUseProgram(m_lightingPassShader.programID());
                break;
            default:
                glUseProgram(m_lightingPassDebugShader.programID());
                assert(m_lightingPassDebugShader.setIndex(static_cast<int>(Renderer::renderMode())));
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
        syncECS();
        drawGBuffer();
        drawLighting();
    }

#ifdef LAVENDER_BUILD_DEBUG
    void
    DeferredRenderer::renderMode(RenderMode renderMode) noexcept {
        Renderer::renderMode(renderMode);
        std::printf("[GL] Renderer: now in render mode %s\n", std::data(toString(renderMode)));
    }
#endif

    bool
    DeferredRenderer::setPointLight(std::size_t index, const ecs::PointLight &pointLight) noexcept {
        return m_lightingPassShader.setPointLight(index, pointLight);
    }

    base::Error
    DeferredRenderer::setup() noexcept {
        if (!m_animatedGBufferShader.setup())
            return errors.error("Setup AnimatedGBuffer shader", "Failed (TODO)");

        if (!m_gBufferShader.setup())
            return errors.error("Setup GBuffer shader", "Failed (TODO)");

        if (!m_lightingPassShader.setup())
            return errors.error("Setup lighting pass shader", "Failed (TODO)");

        if (!m_renderQuad.generate())
            return errors.error("Generate render quad", "Failed (TODO)");

#ifdef LAVENDER_BUILD_DEBUG
        if (!setupDebugEnvironment())
            return errors.error("Setup debug environment", "Failed (TODO)");
        
#endif // LAVENDER_BUILD_DEBUG

        return base::Error::success();
    }

#ifdef LAVENDER_BUILD_DEBUG
    bool
    DeferredRenderer::setupDebugEnvironment() noexcept {
        return m_lightingPassDebugShader.setup();
    }
#endif // LAVENDER_BUILD_DEBUG

    void
    DeferredRenderer::syncECS() noexcept {
        if (core()->scene()->entityList().updateCount() == m_ecsUpdateCount)
            return;
        m_ecsUpdateCount = core()->scene()->entityList().updateCount();

        // TODO this should be optimized in so many ways
        std::size_t pointLightIndex{0};
        for (const auto &entity : std::data(core()->scene()->entityList())) {
            if (!entity->isLight())
                continue;

            const auto *pointLight = static_cast<const ecs::PointLight *>(entity.get());
            const auto result = m_lightingPassShader.setPointLight(pointLightIndex++, *pointLight);
            assert(result);
            static_cast<void>(result);
        }

        // TODO in the future, when we have less lights than the light limit,
        //      we should reset the data in the shader.
    }

} // namespace gle
