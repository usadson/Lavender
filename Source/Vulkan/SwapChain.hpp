/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Source/Vulkan/IncludeVulkan.hpp"
#include "Source/Vulkan/SwapChainElement.hpp"

namespace vke {

    class Core;

    class SwapChain {
        Core *m_core{};
        
		vk::SwapchainKHR m_swapChain{};
		vk::Extent2D m_swapChainExtent{};
		vk::Format m_swapChainImageFormat{};
		std::vector<SwapChainElement> m_swapChainElements{};

		[[nodiscard]] vk::Extent2D
		chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) const noexcept;

		[[nodiscard]] vk::PresentModeKHR
		chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) const noexcept;

		[[nodiscard]] vk::SurfaceFormatKHR
		chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) const noexcept;

		[[nodiscard]] bool
		createImageViews() noexcept;

    public:
        [[nodiscard]] inline
        SwapChain(Core *core) noexcept
            : m_core(core) {
        }

        ~SwapChain() noexcept;

        [[nodiscard]] bool
        create();

		[[nodiscard]] bool
		createFramebuffers(vk::RenderPass renderPass) noexcept;

		[[nodiscard]] inline constexpr vk::Extent2D
		extent() const noexcept {
			return m_swapChainExtent;
		}

		[[nodiscard]] inline constexpr vk::SwapchainKHR
		handle() const noexcept {
			return m_swapChain;
		}

		[[nodiscard]] inline constexpr vk::Format
		imageFormat() const noexcept {
			return m_swapChainImageFormat;
		}

		[[nodiscard]] inline constexpr std::vector<SwapChainElement> &
		elements() noexcept {
			return m_swapChainElements;
		}

		[[nodiscard]] inline constexpr const std::vector<SwapChainElement> &
		elements() const noexcept {
			return m_swapChainElements;
		}
    };

} // namespace vke
