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

#include "Source/Resources/ModelDescriptor.hpp"
#include "Source/Resources/ModelGeometry.hpp"
#include "Source/Resources/TextureDescriptor.hpp"
#include "Source/Resources/TextureInput.hpp"

class WindowAPI;

class GraphicsAPI {
public:
    enum class Name {
        OPENGL,
        VULKAN,
    };

    [[nodiscard]] virtual resources::ModelGeometryDescriptor *
    createModelGeometry(const resources::ModelGeometry &geometry) noexcept = 0;

    [[nodiscard]] virtual resources::TextureDescriptor *
    createTexture(const resources::TextureInput &textureInput) noexcept = 0;

    [[nodiscard]] virtual bool
    initialize(WindowAPI *) = 0;

    virtual void
    renderEntities() = 0;

    virtual ~GraphicsAPI() noexcept = default;
};
