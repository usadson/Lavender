/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <string_view>

#include "Source/Window/WindowAPI.hpp"

struct GLFWwindow;

namespace window {

    class GLFWCore
            : public WindowAPI {

        [[nodiscard]] std::string_view
        getGLFWError() const noexcept;

        GLFWwindow *m_window{nullptr};

    public:
        [[nodiscard]] bool
        initialize(GraphicsAPI::Name graphicsAPI) override;

        ~GLFWCore() override;

#ifdef ENABLE_VULKAN
        [[nodiscard]] bool
        createVulkanSurface(vk::Instance instance,
                            const vk::Optional<const vk::AllocationCallbacks> &allocator,
                            vk::SurfaceKHR *surfaceDestination) noexcept override;
    
        [[nodiscard]] base::ArrayView<const char *const>
        getRequiredVulkanInstanceExtensions() noexcept override;
#endif
        
        [[nodiscard]] math::Vector2u
        queryFramebufferSize() const noexcept override;

        [[nodiscard]] utils::Version<int>
        queryGLContextVersion() const noexcept override;

        /**
         * Request to the underlying API to enable or disable VSync. Please note
         * that this is just a request, and doesn't have to be honored. The
         * OpenGL driver or OS may reject this request if it wants to.
         */
        void
        requestVSyncMode(bool enabled) noexcept override;

        [[nodiscard]] bool
        shouldClose() override;

        void preLoop() override;
        void postLoop() override;
    };

} // namespace window
