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
#include "Source/OpenGL/Renderer/Renderer.hpp"
#include "Source/OpenGL/Renderer/SkyBoxRenderer.hpp"
#include "Source/OpenGL/Resources/SkinDescriptor.hpp"
#include "Source/OpenGL/Resources/SkyboxDescriptor.hpp"
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
        std::vector<std::unique_ptr<SkyboxDescriptor>> m_skyBoxDescriptors{};
        std::vector<std::unique_ptr<SkinDescriptor>> m_skinDescriptor{};

        std::unique_ptr<Renderer> m_renderer{};
        std::unique_ptr<SkyBoxRenderer> m_skyBoxRenderer{};

        [[nodiscard]] std::optional<unsigned int>
        createElementBuffer(const std::vector<resources::ModelGeometry::IndexType> &) const noexcept;

        [[nodiscard]] std::optional<unsigned int>
        createTextureBuffer(const std::vector<resources::ModelGeometry::TextureCoordType> &) const noexcept;

        [[nodiscard]] std::optional<unsigned int>
        createVertexBuffer(const std::vector<resources::ModelGeometry::VertexType> &) const noexcept;

        [[nodiscard]] static base::Error
        initializeGLEW() noexcept;

    public:
        [[nodiscard]] inline explicit
        Core(const ecs::Scene *scene,
             const input::Controller *controller,
             const interface::Camera *camera) noexcept
                : GraphicsAPI(scene, controller, camera) {
        }

        [[nodiscard]] base::ErrorOr<resources::ModelGeometryDescriptor *>
        createModelGeometry(const resources::ModelGeometry &geometry) noexcept override;

        [[nodiscard]] base::ErrorOr<resources::SkyboxDescriptor *>
        createSkyBox(const resources::TextureInput &) noexcept override;

        [[nodiscard]] base::ErrorOr<resources::ModelGeometryDescriptor *>
        createSphere(std::size_t stackCount, std::size_t sectorCount, float radius) noexcept override;

        [[nodiscard]] base::ErrorOr<resources::TextureDescriptor *>
        createTexture(const resources::TextureInput &textureInput) noexcept override;

        [[nodiscard]] base::Error
        initialize(WindowAPI *) override;

        [[nodiscard]] base::ErrorOr<std::unique_ptr<ecs::Scene>>
        loadGLTFScene(std::string_view fileName) noexcept override;

        /**
         * Is called to initialize things that depend on the window size.
         *
         * TODO: Should be called when the window resizes.
         */
        void
        onResize(math::Size2D<std::uint32_t>) noexcept override;

        void
        renderEntities() noexcept override;

        [[nodiscard]] base::ErrorOr<const resources::ModelDescriptor *>
        uploadModelDescriptor(resources::ModelDescriptor &&modelDescriptor) noexcept override;
    };

} // namespace gle
