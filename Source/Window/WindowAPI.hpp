/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <functional>

#include <cstdint>

#include "Source/Base/ArrayView.hpp"
#include "Source/Base/Error.hpp"
#include "Source/Base/Library.hpp"
#include "Source/Event/EventHandler.hpp"
#include "Source/GraphicsAPI.hpp"
#include "Source/Input/KeyboardUpdate.hpp"
#include "Source/Input/MouseGrabbed.hpp"
#include "Source/Input/MouseUpdate.hpp"
#include "Source/Math/Size2D.hpp"
#include "Source/Math/Vector.hpp"
#include "Source/Utils/Version.hpp"
#include "Source/Window/Events.hpp"
#include "Source/Window/Options.hpp"

#ifdef ENABLE_VULKAN
#include "Source/Vulkan/IncludeVulkan.hpp"
#endif

class WindowAPI {
public:
    using KeyboardCallbackType = std::function<void(input::KeyboardUpdate)>;
    using MouseCallbackType = std::function<void(input::MouseUpdate)>;

private:
    GraphicsAPI *m_graphicsAPI{nullptr};
    KeyboardCallbackType m_keyboardCallback{};
    MouseCallbackType m_mouseCallback{};
    DarkModeOption m_darkModeOption{DarkModeOption::HONOR_USER_SETTINGS};
    WindowVisibilityOption m_windowVisibilityOption{WindowVisibilityOption::HIDE};

protected:
    input::MouseGrabbed m_mouseGrabbed{input::MouseGrabbed::NO};

    virtual void 
    onVisibilityOptionUpdated() noexcept = 0;

    [[nodiscard]] inline 
    WindowAPI() noexcept = default;

public:
    //
    // Events
    // 

    event::EventHandler<window::CloseRequestedEvent> onCloseRequested{};
    event::EventHandler<window::FocusGainedEvent> onFocusGained{};
    event::EventHandler<window::FocusLostEvent> onFocusLost{};
    event::EventHandler<window::ResizeEvent> onResize{};
    event::EventHandler<window::VisibilityChangedEvent> onVisibilityChanged{};

    event::EventHandler<input::KeyPressedEvent> onKeyPressed{};
    event::EventHandler<input::KeyReleasedEvent> onKeyReleased{};

    //
    // Functions
    //

    [[nodiscard]] virtual base::Error
    initialize(GraphicsAPI::Name graphicsAPI) = 0;

    virtual
    ~WindowAPI() = default;

#ifdef ENABLE_VULKAN
    [[nodiscard]] virtual base::ArrayView<const char *const>
    getRequiredVulkanInstanceExtensions() noexcept = 0;

    [[nodiscard]] virtual bool
    createVulkanSurface(vk::Instance instance,
        const vk::Optional<const vk::AllocationCallbacks> &allocator,
        vk::SurfaceKHR *surfaceDestination) noexcept = 0;
#endif

    [[nodiscard]] inline constexpr DarkModeOption 
    darkMode() const noexcept { 
        return m_darkModeOption;
    }

    [[nodiscard]] virtual bool 
    doesWindowSupportDarkMode() const noexcept = 0;

    [[nodiscard]] inline constexpr GraphicsAPI *
    graphicsAPI() noexcept {
        return m_graphicsAPI;
    }

    [[nodiscard]] inline constexpr const KeyboardCallbackType &
    keyboardCallback() const noexcept {
        return m_keyboardCallback;
    }

    [[nodiscard]] inline constexpr const MouseCallbackType &
    mouseCallback() const noexcept {
        return m_mouseCallback;
    }

    [[nodiscard]] inline constexpr input::MouseGrabbed
    mouseGrabbed() const noexcept {
        return m_mouseGrabbed;
    }

    [[nodiscard]] virtual math::Vector2u
    queryFramebufferSize() const noexcept = 0;

    [[nodiscard]] virtual utils::Version<int>
    queryGLContextVersion() const noexcept = 0;

    virtual inline void
    registerGraphicsAPI(GraphicsAPI *graphicsAPI) noexcept {
        m_graphicsAPI = graphicsAPI;
    }

    virtual inline void
    registerKeyboardCallback(KeyboardCallbackType &&callback) noexcept {
        m_keyboardCallback = std::move(callback);
    }

    virtual inline void
    registerMouseCallback(MouseCallbackType &&callback) noexcept {
        m_mouseCallback = std::move(callback);
    }

    [[nodiscard]] virtual base::Error
    requestClose(window::CloseRequestedEvent::Reason reason) noexcept = 0;

    virtual void
    requestVSyncMode(bool enabled) noexcept = 0;

    [[nodiscard]] virtual base::Error 
    setIcon(std::string_view fileName) noexcept = 0;

    virtual inline void 
    setDarkMode(DarkModeOption option) noexcept {
        m_darkModeOption = option;
    }

    virtual inline void
    setMouseGrabbed(input::MouseGrabbed mouseGrabbed) noexcept {
        m_mouseGrabbed = mouseGrabbed;
    }

    inline void 
    setVisibility(WindowVisibilityOption visibilityOption) noexcept {
        const auto oldValue = m_windowVisibilityOption;
        m_windowVisibilityOption = visibilityOption;

        window::VisibilityChangedEvent event{this, oldValue, visibilityOption};
        if (auto error = onVisibilityChanged.invoke(event))
            error.displayErrorMessageBox();
        
        onVisibilityOptionUpdated();
    }

    [[nodiscard]] virtual bool
    shouldClose() = 0;

    /**
     * This function is called at the beginning of each game loop cycle. This
     * function is often used to poll the underlying window for events (close
     * request, resize, etc).
     */
    virtual void
    preLoop() = 0;

    /**
     * This function is called at after each game loop cycle. This function is
     * often used to swap buffers.
     */
    virtual void
    postLoop() = 0;

    [[nodiscard]] inline constexpr WindowVisibilityOption 
    visibility() const noexcept { 
        return m_windowVisibilityOption;
    }
};
