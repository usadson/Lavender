/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Source/Base/ArrayView.hpp"
#include "Source/Math/Vector.hpp"

#ifdef ENABLE_VULKAN
#   include "Source/Vulkan/IncludeVulkan.hpp"
#endif

class WindowAPI {
public:
	[[nodiscard]] virtual bool
	initialize() = 0;

	virtual ~WindowAPI() = default;

#ifdef ENABLE_VULKAN
    [[nodiscard]] virtual base::ArrayView<const char *const>
	getRequiredVulkanInstanceExtensions() noexcept = 0;

	[[nodiscard]] virtual bool
	createVulkanSurface(vk::Instance instance,
						const vk::Optional<const vk::AllocationCallbacks> &allocator,
						vk::SurfaceKHR *surfaceDestination) noexcept = 0;
#endif

	[[nodiscard]] virtual math::Vector2u
	queryFramebufferSize() const noexcept = 0;

	[[nodiscard]] virtual bool
	shouldClose() = 0;

	/**
     * This function is called at the beginning of each game loop cycle. This
	 * function is often used to poll the underlying window for events (close
	 * request, resize, etc).
     */
	virtual void preLoop() = 0;

	/**
     * This function is called at after each game loop cycle. This function is
     * often used to swap buffers.
     */
    virtual void postLoop() = 0;
};
