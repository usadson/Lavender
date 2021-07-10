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

#include "Source/ECS/EntityList.hpp"
#include "Source/Resources/ModelDescriptor.hpp"
#include "Source/Resources/ModelGeometry.hpp"
#include "Source/Resources/TextureDescriptor.hpp"
#include "Source/Resources/TextureInput.hpp"

class WindowAPI;

class GraphicsAPI {
protected:
    const ecs::EntityList *m_entityList;
public:
    [[nodiscard]] inline constexpr explicit
    GraphicsAPI(const ecs::EntityList *entityList) noexcept
            : m_entityList(entityList) {
    }

    enum class Name {
        OPENGL,
        VULKAN,
    };

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

    virtual void
    renderEntities() = 0;

    [[nodiscard]] virtual resources::ModelDescriptor *
    uploadModelDescriptor(resources::ModelDescriptor &&modelDescriptor) noexcept = 0;

    virtual ~GraphicsAPI() noexcept = default;
};
