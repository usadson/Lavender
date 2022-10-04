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
            case GLFW_KEY_A: return input::KeyboardKey::A;
            case GLFW_KEY_B: return input::KeyboardKey::B;
            case GLFW_KEY_C: return input::KeyboardKey::C;
            case GLFW_KEY_D: return input::KeyboardKey::D;
            case GLFW_KEY_Q: return input::KeyboardKey::Q;
            case GLFW_KEY_E: return input::KeyboardKey::E;
            case GLFW_KEY_F: return input::KeyboardKey::F;
            case GLFW_KEY_G: return input::KeyboardKey::G;
            case GLFW_KEY_H: return input::KeyboardKey::H;
            case GLFW_KEY_I: return input::KeyboardKey::I;
            case GLFW_KEY_J: return input::KeyboardKey::J;
            case GLFW_KEY_K: return input::KeyboardKey::K;
            case GLFW_KEY_L: return input::KeyboardKey::L;
            case GLFW_KEY_M: return input::KeyboardKey::M;
            case GLFW_KEY_N: return input::KeyboardKey::N;
            case GLFW_KEY_O: return input::KeyboardKey::O;
            case GLFW_KEY_P: return input::KeyboardKey::P;
            case GLFW_KEY_R: return input::KeyboardKey::R;
            case GLFW_KEY_S: return input::KeyboardKey::S;
            case GLFW_KEY_T: return input::KeyboardKey::T;
            case GLFW_KEY_U: return input::KeyboardKey::U;
            case GLFW_KEY_V: return input::KeyboardKey::V;
            case GLFW_KEY_W: return input::KeyboardKey::W;
            case GLFW_KEY_X: return input::KeyboardKey::X;
            case GLFW_KEY_Y: return input::KeyboardKey::Y;
            case GLFW_KEY_Z: return input::KeyboardKey::Z;

            case GLFW_KEY_SPACE: return input::KeyboardKey::SPACE;
            case GLFW_KEY_COMMA: return input::KeyboardKey::COMMA;
            case GLFW_KEY_PERIOD: return input::KeyboardKey::FULL_STOP;
            case GLFW_KEY_SEMICOLON: return input::KeyboardKey::SEMICOLON;
            case GLFW_KEY_APOSTROPHE: return input::KeyboardKey::APOSTROPHE;
            case GLFW_KEY_SLASH: return input::KeyboardKey::SOLIDUS;
            case GLFW_KEY_BACKSLASH: return input::KeyboardKey::REVERSE_SOLIDUS;
            case GLFW_KEY_LEFT_BRACKET: return input::KeyboardKey::LEFT_SQUARE_BRACKET;
            case GLFW_KEY_RIGHT_BRACKET: return input::KeyboardKey::RIGHT_SQUARE_BRACKET;
            case GLFW_KEY_MINUS: return input::KeyboardKey::HYPHEN_MINUS;

            case GLFW_KEY_TAB: return input::KeyboardKey::TAB;
            case GLFW_KEY_CAPS_LOCK: return input::KeyboardKey::CAPS_LOCK;
            case GLFW_KEY_ENTER: return input::KeyboardKey::ENTER;

            case GLFW_KEY_LEFT_SHIFT: return input::KeyboardKey::LEFT_SHIFT;
            case GLFW_KEY_RIGHT_SHIFT: return input::KeyboardKey::RIGHT_SHIFT;
            case GLFW_KEY_LEFT_CONTROL: return input::KeyboardKey::LEFT_CONTROL;
            case GLFW_KEY_RIGHT_CONTROL: return input::KeyboardKey::RIGHT_CONTROL;
            case GLFW_KEY_LEFT_ALT: return input::KeyboardKey::LEFT_ALT;
            case GLFW_KEY_RIGHT_ALT: return input::KeyboardKey::RIGHT_ALT;
            case GLFW_KEY_LEFT_SUPER: return input::KeyboardKey::LEFT_META;
            case GLFW_KEY_RIGHT_SUPER: return input::KeyboardKey::RIGHT_META;

            case GLFW_KEY_0: return input::KeyboardKey::DIGIT0;
            case GLFW_KEY_1: return input::KeyboardKey::DIGIT1;
            case GLFW_KEY_2: return input::KeyboardKey::DIGIT2;
            case GLFW_KEY_3: return input::KeyboardKey::DIGIT3;
            case GLFW_KEY_4: return input::KeyboardKey::DIGIT4;
            case GLFW_KEY_5: return input::KeyboardKey::DIGIT5;
            case GLFW_KEY_6: return input::KeyboardKey::DIGIT6;
            case GLFW_KEY_7: return input::KeyboardKey::DIGIT7;
            case GLFW_KEY_8: return input::KeyboardKey::DIGIT8;
            case GLFW_KEY_9: return input::KeyboardKey::DIGIT9;

            case GLFW_KEY_F1: return input::KeyboardKey::F1;
            case GLFW_KEY_F2: return input::KeyboardKey::F2;
            case GLFW_KEY_F3: return input::KeyboardKey::F3;
            case GLFW_KEY_F4: return input::KeyboardKey::F4;
            case GLFW_KEY_F5: return input::KeyboardKey::F5;
            case GLFW_KEY_F6: return input::KeyboardKey::F6;
            case GLFW_KEY_F7: return input::KeyboardKey::F7;
            case GLFW_KEY_F8: return input::KeyboardKey::F8;
            case GLFW_KEY_F9: return input::KeyboardKey::F9;
            case GLFW_KEY_F10: return input::KeyboardKey::F10;
            case GLFW_KEY_F11: return input::KeyboardKey::F11;
            case GLFW_KEY_F12: return input::KeyboardKey::F12;
            case GLFW_KEY_F13: return input::KeyboardKey::F13;
            case GLFW_KEY_F14: return input::KeyboardKey::F14;
            case GLFW_KEY_F15: return input::KeyboardKey::F15;
            case GLFW_KEY_F16: return input::KeyboardKey::F16;
            case GLFW_KEY_F17: return input::KeyboardKey::F17;
            case GLFW_KEY_F18: return input::KeyboardKey::F18;
            case GLFW_KEY_F19: return input::KeyboardKey::F19;
            case GLFW_KEY_F20: return input::KeyboardKey::F20;
            case GLFW_KEY_F21: return input::KeyboardKey::F21;
            case GLFW_KEY_F22: return input::KeyboardKey::F22;
            case GLFW_KEY_F23: return input::KeyboardKey::F23;
            case GLFW_KEY_F24: return input::KeyboardKey::F24;
            case GLFW_KEY_F25: return input::KeyboardKey::F25;

            case GLFW_KEY_KP_ADD: return input::KeyboardKey::NUMPAD_PLUS;
            case GLFW_KEY_KP_SUBTRACT: return input::KeyboardKey::NUMPAD_MINUS;
            case GLFW_KEY_KP_DIVIDE: return input::KeyboardKey::NUMPAD_DIVIDE;
            case GLFW_KEY_KP_MULTIPLY: return input::KeyboardKey::NUMPAD_MULTIPLY;
            case GLFW_KEY_KP_DECIMAL: return input::KeyboardKey::NUMPAD_DECIMAL;
            case GLFW_KEY_KP_ENTER: return input::KeyboardKey::NUMPAD_ENTER;
            case GLFW_KEY_KP_0: return input::KeyboardKey::NUMPAD0;
            case GLFW_KEY_KP_1: return input::KeyboardKey::NUMPAD1;
            case GLFW_KEY_KP_2: return input::KeyboardKey::NUMPAD2;
            case GLFW_KEY_KP_3: return input::KeyboardKey::NUMPAD3;
            case GLFW_KEY_KP_4: return input::KeyboardKey::NUMPAD4;
            case GLFW_KEY_KP_5: return input::KeyboardKey::NUMPAD5;
            case GLFW_KEY_KP_6: return input::KeyboardKey::NUMPAD6;
            case GLFW_KEY_KP_7: return input::KeyboardKey::NUMPAD7;
            case GLFW_KEY_KP_8: return input::KeyboardKey::NUMPAD8;
            case GLFW_KEY_KP_9: return input::KeyboardKey::NUMPAD9;

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
    mouseButtonCallbackGLFW(GLFWwindow *window, int button, int action, int mods) noexcept {
        auto *core = reinterpret_cast<GLFWCore *>(glfwGetWindowUserPointer(window));
        if (core == nullptr || !core->mouseCallback())
            return;

        static_cast<void>(mods);

        if (core->mouseGrabbed() == input::MouseGrabbed::YES) {
            if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
                core->setMouseGrabbed(input::MouseGrabbed::NO);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                return;
            }

            switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                core->mouseCallback()({input::MouseButton::LEFT, action == GLFW_PRESS});
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                core->mouseCallback()({input::MouseButton::MIDDLE, action == GLFW_PRESS});
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                core->mouseCallback()({input::MouseButton::RIGHT, action == GLFW_PRESS});
                break;
            default:
                break;
            }
        } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            core->setMouseGrabbed(input::MouseGrabbed::YES);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    static void
    mousePositionCallbackGLFW(GLFWwindow *window, double posX, double posY) noexcept {
        auto *core = reinterpret_cast<GLFWCore *>(glfwGetWindowUserPointer(window));
        if (core == nullptr || core->mouseGrabbed() == input::MouseGrabbed::NO || !core->mouseCallback())
            return;

        int middleX, middleY;
        glfwGetWindowSize(window, &middleX, &middleY);

        middleX /= 2;
        middleY /= 2;

        const double deltaX = middleX - posX;
        const double deltaY = middleY - posY;

        core->mouseCallback()({input::MouseButton::NONE, false, static_cast<float>(deltaX), static_cast<float>(deltaY)});

        glfwSetCursorPos(window, middleX, middleY);
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

    base::Error
    GLFWCore::initialize(GraphicsAPI::Name graphicsAPI) {
        if (m_window != nullptr)
            return base::Error::success();

        base::FunctionErrorGenerator errors{"WindowAPI", "GLFWCore"};
        if (!glfwInit())
            return errors.error("glfwInit", getGLFWError());

        glfwWindowHint(GLFW_CLIENT_API, convertGraphicsAPINameToGLFWEnum(graphicsAPI));
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        if (graphicsAPI == GraphicsAPI::Name::OPENGL) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }

        m_window = glfwCreateWindow(1280, 720, "Lavender", nullptr, nullptr);
        if (m_window == nullptr)
            return errors.error("glfwCreateWindow", getGLFWError());

        if (graphicsAPI == GraphicsAPI::Name::OPENGL) {
            glfwMakeContextCurrent(m_window);
#ifndef NDEBUG
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
        }

        glfwSetWindowUserPointer(m_window, this);

        glfwSetKeyCallback(m_window, reinterpret_cast<GLFWkeyfun>(&keyboardCallbackGLFW));
        glfwSetMouseButtonCallback(m_window, reinterpret_cast<GLFWmousebuttonfun>(&mouseButtonCallbackGLFW));
        glfwSetCursorPosCallback(m_window, reinterpret_cast<GLFWcursorposfun>(&mousePositionCallbackGLFW));
        glfwSetFramebufferSizeCallback(m_window, &resizeCallback);

        return base::Error::success();
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

    base::Error 
    GLFWCore::requestClose(window::CloseRequestedEvent::Reason reason) noexcept {
        CloseRequestedEvent event{this, reason};

        if (auto error = onCloseRequested.invoke(event))
            return error;

        if (event.cancelStatus() != event::CancelStatus::NOT_CANCELLED)
            return base::Error::success();

        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
        return base::Error::success();
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
        return glfwWindowShouldClose(m_window) == GLFW_TRUE;
    }

    void
    GLFWCore::onVisibilityOptionUpdated() noexcept { 
        switch (visibility()) {
            case WindowVisibilityOption::HIDE:
                glfwHideWindow(m_window);
                break;
            case WindowVisibilityOption::SHOW:
                glfwShowWindow(m_window);
                break;
        }
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
