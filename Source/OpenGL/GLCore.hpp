/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Vulkan and the Vulkan logo are registered trademarks of the Khronos Group
 * Inc.
 *
 * vke is an abbreviation for VulKan Engine.
 */

#pragma once

#include <memory>
#include <optional>
#include <vector>

#include <cstdint>

#include "Source/GraphicsAPI.hpp"
#include "Source/Math/Size2D.hpp"
#include "Source/OpenGL/ModelGeometryDescriptor.hpp"
#include "Source/OpenGL/Resources/GBuffer.hpp"
#include "Source/OpenGL/Resources/RenderQuad.hpp"
#include "Source/OpenGL/Shaders/LightingPassShader.hpp"
#include "Source/OpenGL/Shaders/ShaderProgram.hpp"
#include "Source/OpenGL/Shaders/Uniform.hpp"
#include "Source/OpenGL/TextureDescriptor.hpp"
#include "Source/Resources/ModelGeometry.hpp"

namespace gle {

    class Core
            : public GraphicsAPI {
        WindowAPI *m_windowAPI{nullptr};

        std::vector<std::unique_ptr<ModelGeometryDescriptor>> m_geometryDescriptors{};
        std::vector<std::unique_ptr<TextureDescriptor>> m_textureDescriptors{};
        std::vector<std::unique_ptr<resources::ModelDescriptor>> m_modelDescriptors{};

        std::unique_ptr<ShaderProgram> m_shaderProgram{};
        GLuint m_shaderAttribPosition{};
        GLuint m_shaderAttribTextureCoordinates{};
        GLuint m_shaderAttribNormal{};

        UniformMatrix4 m_uniformTransformation{-1};
        UniformMatrix4 m_uniformProjection{-1};
        UniformMatrix4 m_uniformView{-1};

        GBuffer m_gBuffer;
        LightingPassShader m_lightingPassShader{};

        RenderQuad m_renderQuad;

        [[nodiscard]] std::optional<unsigned int>
        createElementBuffer(const std::vector<resources::ModelGeometry::IndexType> &) const noexcept;

        [[nodiscard]] bool
        createLights() noexcept;

        [[nodiscard]] std::optional<unsigned int>
        createTextureBuffer(const std::vector<resources::ModelGeometry::TextureCoordType> &) const noexcept;

        [[nodiscard]] std::optional<unsigned int>
        createVertexBuffer(const std::vector<resources::ModelGeometry::VertexType> &) const noexcept;

        [[nodiscard]] static bool
        initializeGLEW() noexcept;

        [[nodiscard]] bool
        setupGeneralShader() noexcept;

    public:
        [[nodiscard]] inline explicit
        Core(const ecs::EntityList *entityList,
             const input::Controller *controller,
             const interface::Camera *camera) noexcept
                : GraphicsAPI(entityList, controller, camera) {
        }

        [[nodiscard]] resources::ModelGeometryDescriptor *
        createModelGeometry(const resources::ModelGeometry &geometry) noexcept override;

        [[nodiscard]] resources::TextureDescriptor *
        createTexture(const resources::TextureInput &textureInput) noexcept override;

        [[nodiscard]] bool
        initialize(WindowAPI *) override;

        [[nodiscard]] resources::ModelGeometryDescriptor *
        loadGLTFModelGeometry(std::string_view fileName) noexcept override;

        /**
         * Is called to initialize things that depend on the window size.
         *
         * TODO: Should be called when the window resizes.
         */
        void
        onResize(math::Size2D<std::uint32_t>) noexcept override;

        void
        renderEntities() noexcept override;

        [[nodiscard]] const resources::ModelDescriptor *
        uploadModelDescriptor(resources::ModelDescriptor &&modelDescriptor) noexcept override;
    };

} // namespace gle
