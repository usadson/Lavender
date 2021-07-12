/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <functional>

#include <cstdint>

#include "Source/Base/ArrayView.hpp"
#include "Source/GraphicsAPI.hpp"
#include "Source/Input/KeyboardUpdate.hpp"
#include "Source/Math/Size2D.hpp"
#include "Source/Math/Vector.hpp"
#include "Source/Utils/Version.hpp"

#ifdef ENABLE_VULKAN
#include "Source/Vulkan/IncludeVulkan.hpp"
#endif

class WindowAPI {
public:
    using KeyboardCallbackType = std::function<void(input::KeyboardUpdate)>;

private:
    GraphicsAPI *m_graphicsAPI{nullptr};
    KeyboardCallbackType m_keyboardCallback{};

public:
    [[nodiscard]] virtual bool
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

    [[nodiscard]] inline constexpr GraphicsAPI *
    graphicsAPI() noexcept {
        return m_graphicsAPI;
    }

    [[nodiscard]] inline constexpr const KeyboardCallbackType &
    keyboardCallback() const noexcept {
        return m_keyboardCallback;
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

    virtual void
    requestVSyncMode(bool enabled) noexcept = 0;

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
};
