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

#include <cstdint>

#include "Source/ECS/EntityList.hpp"
#include "Source/Math/Size2D.hpp"
#include "Source/Input/Forward.hpp"
#include "Source/Interface/Forward.hpp"
#include "Source/Resources/ModelDescriptor.hpp"
#include "Source/Resources/ModelGeometry.hpp"
#include "Source/Resources/TextureDescriptor.hpp"
#include "Source/Resources/TextureInput.hpp"

#include <string_view>

class WindowAPI;

class GraphicsAPI {
protected:
    const ecs::EntityList *m_entityList;

    const input::Controller *m_controller;
    const interface::Camera *m_camera;
public:
    [[nodiscard]] inline constexpr explicit
    GraphicsAPI(const ecs::EntityList *entityList,
                const input::Controller *controller,
                const interface::Camera *camera) noexcept
            : m_entityList(entityList)
            , m_controller(controller)
            , m_camera(camera) {
    }

    enum class Name {
        OPENGL,
        VULKAN,
    };

    [[nodiscard]] inline const interface::Camera *
    camera() const noexcept {
        return m_camera;
    }

    [[nodiscard]] inline const input::Controller *
    controller() const noexcept {
        return m_controller;
    }

    [[nodiscard]] virtual resources::ModelGeometryDescriptor *
    createModelGeometry(const resources::ModelGeometry &geometry) noexcept = 0;

    [[nodiscard]] virtual resources::TextureDescriptor *
    createTexture(const resources::TextureInput &textureInput) noexcept = 0;

    [[nodiscard]] inline constexpr const ecs::EntityList *
    entityList() const noexcept {
        return m_entityList;
    }

    [[nodiscard]] virtual bool
    initialize(WindowAPI *) = 0;

    [[nodiscard]] virtual inline resources::ModelGeometryDescriptor *
    loadGLTFModelGeometry(std::string_view fileName) noexcept {
        static_cast<void>(fileName);
        return nullptr;
    }

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
    [[nodiscard]] virtual const resources::ModelDescriptor *
    uploadModelDescriptor(resources::ModelDescriptor &&modelDescriptor) noexcept = 0;

    virtual ~GraphicsAPI() noexcept = default;
};
