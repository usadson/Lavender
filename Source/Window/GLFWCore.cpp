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

    [[nodiscard]] inline static constexpr input::KeyboardKey
    translateGLFWKeyToInputKeyboardKey(int key) noexcept {
        switch (key) {
            case GLFW_KEY_W: return input::KeyboardKey::W;
            case GLFW_KEY_A: return input::KeyboardKey::A;
            case GLFW_KEY_S: return input::KeyboardKey::S;
            case GLFW_KEY_D: return input::KeyboardKey::D;
            case GLFW_KEY_LEFT_SHIFT: return input::KeyboardKey::LEFT_SHIFT;
            case GLFW_KEY_LEFT_CONTROL: return input::KeyboardKey::LEFT_CONTROL;
            case GLFW_KEY_SPACE: return input::KeyboardKey::SPACE;
            default: return input::KeyboardKey::INVALID_KEY;
        }
    }

    [[nodiscard]] inline static constexpr input::KeyboardAction
    translateGLFWActionToKeyboardAction(int action) noexcept {
        switch (action) {
        case GLFW_PRESS: return input::KeyboardAction::PRESS;
        case GLFW_RELEASE: return input::KeyboardAction::RELEASE;
        case GLFW_REPEAT: return input::KeyboardAction::REPEAT;
        default:
            assert(false);
            return input::KeyboardAction::INVALID;
        }
    }

    /**
     * action is one of:
     *   GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
     */
    static void
    keyboardCallbackGLFW(GLFWwindow *window, int key, int scanCode, int action, int mods) noexcept {
        static_cast<void>(scanCode);
        static_cast<void>(mods);

        if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        const auto inputKey = translateGLFWKeyToInputKeyboardKey(key);
        const auto inputAction = translateGLFWActionToKeyboardAction(action);
        if (inputKey == input::KeyboardKey::INVALID_KEY || inputAction == input::KeyboardAction::INVALID)
            return;

        auto *core = reinterpret_cast<GLFWCore *>(glfwGetWindowUserPointer(window));
        if (core == nullptr || !core->keyboardCallback())
            return;

        core->keyboardCallback()(input::KeyboardUpdate{inputKey, inputAction});
    }

    static void
    resizeCallback(GLFWwindow *window, int width, int height) noexcept {
        assert(width > 0);
        assert(height > 0);

        auto *graphicsAPI = reinterpret_cast<GLFWCore *>(glfwGetWindowUserPointer(window))->graphicsAPI();
        if (graphicsAPI) {
            graphicsAPI->onResize({static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)});
        }
    }

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
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        if (graphicsAPI == GraphicsAPI::Name::OPENGL) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }

        m_window = glfwCreateWindow(1280, 720, "Lavender", nullptr, nullptr);

        if (graphicsAPI == GraphicsAPI::Name::OPENGL) {
            glfwMakeContextCurrent(m_window);
#ifndef NDEBUG
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
        }

        glfwSetWindowUserPointer(m_window, this);

        glfwSetKeyCallback(m_window, reinterpret_cast<GLFWkeyfun>(&keyboardCallbackGLFW));
        glfwSetFramebufferSizeCallback(m_window, &resizeCallback);

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

    utils::Version<int>
    GLFWCore::queryGLContextVersion() const noexcept {
        return {
            glfwGetWindowAttrib(m_window, GLFW_CONTEXT_VERSION_MAJOR),
            glfwGetWindowAttrib(m_window, GLFW_CONTEXT_VERSION_MINOR),
            glfwGetWindowAttrib(m_window, GLFW_CONTEXT_REVISION)
        };
    }

    void
    GLFWCore::requestVSyncMode(bool enabled) noexcept {
        if (enabled) {
            glfwSwapInterval(1);
        } else {
            glfwSwapInterval(0);
        }
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
        glfwSwapBuffers(m_window);
    }

} // namespace window
