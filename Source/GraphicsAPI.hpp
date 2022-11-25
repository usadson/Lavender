/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Vulkan and the Vulkan logo are registered trademarks of the Khronos Group
 * Inc.
 *
 * OpenGL® and the oval logo are trademarks or registered trademarks of Hewlett
 * Packard Enterprise in the United States and/or other countries worldwide.
 */

#pragma once

#include <memory>

#include <cstdint>

#include "Source/Base/Debug.hpp"
#include "Source/Base/ErrorOr.hpp"
#include "Source/ECS/Forward.hpp"
#include "Source/Event/EventHandler.hpp"
#include "Source/Math/Size2D.hpp"
#include "Source/Input/Forward.hpp"
#include "Source/Input/KeyboardUpdate.hpp"
#include "Source/Interface/Forward.hpp"
#include "Source/Resources/ModelDescriptor.hpp"
#include "Source/Resources/ModelGeometry.hpp"
#include "Source/Resources/ResourceLocateEvent.hpp"
#include "Source/Resources/SkyboxDescriptor.hpp"
#include "Source/Resources/TextureDescriptor.hpp"
#include "Source/Resources/TextureInput.hpp"

class WindowAPI;

class GraphicsAPI {
protected:
    const ecs::Scene *m_scene;

    const input::Controller *m_controller;
    const interface::Camera *m_camera;
public:
    [[nodiscard]] inline constexpr explicit
    GraphicsAPI(const ecs::Scene *scene,
                const input::Controller *controller,
                const interface::Camera *camera) noexcept
            : m_scene(scene)
            , m_controller(controller)
            , m_camera(camera) {
    }

    enum class Name {
        OPENGL,
        VULKAN,
    };

    event::EventHandler<resources::ResourceLocateEvent> onLocateResource{};

#ifdef LAVENDER_BUILD_DEBUG
    event::EventHandler<const std::string> onGraphicsModeChange{};
#endif // LAVENDER_BUILD_DEBUG

    [[nodiscard]] inline const interface::Camera *
    camera() const noexcept {
        return m_camera;
    }

    [[nodiscard]] inline const input::Controller *
    controller() const noexcept {
        return m_controller;
    }

    [[nodiscard]] virtual base::ErrorOr<resources::ModelGeometryDescriptor *>
    createModelGeometry(const resources::ModelGeometry &geometry) noexcept = 0;

    [[nodiscard]] virtual base::ErrorOr<resources::SkyboxDescriptor *>
    createSkyBox(const resources::TextureInput &textureInput) noexcept = 0;

    [[nodiscard]] virtual base::ErrorOr<resources::ModelGeometryDescriptor *>
    createSphere(std::size_t stackCount, std::size_t sectorCount, float radius) noexcept;

    [[nodiscard]] virtual base::ErrorOr<resources::TextureDescriptor *>
    createTexture(const resources::TextureInput &textureInput) noexcept = 0;

    [[nodiscard]] inline constexpr const ecs::Scene *
    scene() const noexcept {
        return m_scene;
    }

    [[nodiscard]] virtual base::Error
    initialize(WindowAPI *) = 0;

    [[nodiscard]] virtual base::ErrorOr<std::unique_ptr<ecs::Scene>>
    loadGLTFScene(std::string_view fileName) noexcept;

#ifdef LAVENDER_BUILD_DEBUG
    virtual void
    onDebugKey(input::KeyboardUpdate) noexcept;
#endif

    virtual inline void
    onResize(math::Size2D<std::uint32_t>) noexcept {}

    virtual void
    renderEntities() = 0;

    /**
     * Returns an immutable reference to the descriptor.
     *
     * It isn't up to the application to modify this value, as it should be
     * communicated with the graphics API whether or not to allocate resources
     * for it.
     *
     * If this descriptor is to be modified after a call to
     * uploadModelDescriptor, there should be another subroutine in GraphicsAPI
     * to do so.
     */
    [[nodiscard]] virtual base::ErrorOr<const resources::ModelDescriptor *>
    uploadModelDescriptor(resources::ModelDescriptor &&modelDescriptor) noexcept = 0;

    virtual ~GraphicsAPI() noexcept = default;
};
