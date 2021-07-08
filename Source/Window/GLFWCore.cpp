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

#include "GLFWCore.hpp"

#include <cassert>
#include <cstdio>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace window {

    [[nodiscard]] inline constexpr int
    convertGraphicsAPINameToGLFWEnum(GraphicsAPI::Name name) noexcept {
        switch (name) {
            case GraphicsAPI::Name::OPENGL:
                return GLFW_OPENGL_API;
            case GraphicsAPI::Name::VULKAN:
            default:
                return GLFW_NO_API;
        }
    }

    bool
    GLFWCore::initialize(GraphicsAPI::Name graphicsAPI) {
        if (!glfwInit()) {
            std::printf("GLFW: failed to init: %s\n", getGLFWError().data());
            return false;
        }

        glfwWindowHint(GLFW_CLIENT_API, convertGraphicsAPINameToGLFWEnum(graphicsAPI));
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = glfwCreateWindow(1280, 720, "Lavender", nullptr, nullptr);

        return true;
    }

    GLFWCore::~GLFWCore() {
        if (m_window) {
            glfwDestroyWindow(m_window);
        }

        glfwTerminate();
    }

    std::string_view
    GLFWCore::getGLFWError() const noexcept {
        const char *description;
        static_cast<void>(glfwGetError(&description));
        return description;
    }

#ifdef ENABLE_VULKAN
    bool
    GLFWCore::createVulkanSurface(vk::Instance instance,
            const vk::Optional<const vk::AllocationCallbacks> &allocator,
            vk::SurfaceKHR *surfaceDestination) noexcept {
        return glfwCreateWindowSurface(instance, m_window,
                   allocator ? reinterpret_cast<const VkAllocationCallbacks *>(
                                    static_cast<const vk::AllocationCallbacks *>(allocator))
                             : nullptr,
                   reinterpret_cast<VkSurfaceKHR *>(surfaceDestination))
            == VK_SUCCESS;
    }

    base::ArrayView<const char *const>
    GLFWCore::getRequiredVulkanInstanceExtensions() noexcept {
        std::uint32_t size{};

        const char **exts = glfwGetRequiredInstanceExtensions(&size);

        return {exts, size};
    }
#endif

    math::Vector2u
    GLFWCore::queryFramebufferSize() const noexcept {
        int width{}, height{};
        glfwGetFramebufferSize(m_window, &width, &height);

        assert(width > 0);
        assert(height > 0);

        return {static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)};
    }

    bool
    GLFWCore::shouldClose() {
        return glfwWindowShouldClose(m_window);
    }

    void
    GLFWCore::preLoop() {
        glfwPollEvents();
    }

    void
    GLFWCore::postLoop() {
        // Nothing yet.
    }

} // namespace window
