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

class WindowAPI;

class GraphicsAPI {
public:
    enum class Name {
        OPENGL,
        VULKAN,
    };

    [[nodiscard]] virtual bool
    initialize(WindowAPI *) = 0;

    virtual ~GraphicsAPI() noexcept = default;
};
