/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * This custom dispatch class helps the dispatch of functions, which
 * aren't guaranteed to exists (EXT, KHR, NV, etc.) suffix, also known
 * as Vulkan extensions.
 */

#pragma once

#include "Source/GraphicsAPI.hpp"

#include <compare>
#include <vector>

#include "Source/Vulkan/IncludeVulkan.hpp"

namespace vke {

	class CustomDispatch {
	public:
        [[nodiscard]] vk::Result
		vkCreateDebugUtilsMessengerEXT(
				vk::Instance instance,
				const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
				const VkAllocationCallbacks *pAllocator,
				VkDebugUtilsMessengerEXT *pMessenger) const VULKAN_HPP_NOEXCEPT {
            const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));

			if (!func)
				return vk::Result::eErrorExtensionNotPresent;

            return static_cast<vk::Result>(func(instance, pCreateInfo, pAllocator, pMessenger));
		}

		void
		vkDestroyDebugUtilsMessengerEXT(vk::Instance instance,
				VkDebugUtilsMessengerEXT debugMessenger,
                const VkAllocationCallbacks *pAllocator) const VULKAN_HPP_NOEXCEPT {
            const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
				instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));

            if (func) {
                func(instance, debugMessenger, pAllocator);
            }
        }
	};

} // namespace vke
