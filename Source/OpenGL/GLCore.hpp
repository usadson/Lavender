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
#include <vector>

#include "Source/GraphicsAPI.hpp"
#include "Source/OpenGL/GraphicsHandle.hpp"
#include "Source/OpenGL/Resources/ShaderProgram.hpp"

namespace gle {

    class Core
            : public GraphicsAPI {
        WindowAPI *m_windowAPI{nullptr};

        std::vector<GraphicsHandle> m_graphicsHandles{};

        std::unique_ptr<ShaderProgram> m_shaderProgram{};
        GLuint m_shaderAttribPosition{};

        [[nodiscard]] static bool
        initializeGLEW() noexcept;

    public:
        [[nodiscard]] resources::GraphicsHandleBase *
        createModel(const resources::ModelGeometry &geometry) noexcept override;

        [[nodiscard]] bool
        initialize(WindowAPI *) override;

        void
        renderEntities() noexcept override;
    };

} // namespace gle
