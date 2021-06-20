/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <vector>

#include "Source/Vulkan/IncludeVulkan.hpp"

namespace vke {

    struct SwapChainSupportDetails {
    private:
        vk::SurfaceCapabilitiesKHR m_capabilities{};
        std::vector<vk::SurfaceFormatKHR> m_formats{};
        std::vector<vk::PresentModeKHR> m_presentModes{};

    public:
        [[nodiscard]] SwapChainSupportDetails() = default;
        [[nodiscard]] SwapChainSupportDetails(SwapChainSupportDetails &&) = default;
        [[nodiscard]] SwapChainSupportDetails(const SwapChainSupportDetails &) = default;
        [[nodiscard]] SwapChainSupportDetails &operator=(SwapChainSupportDetails &&) = default;
        [[nodiscard]] SwapChainSupportDetails &operator=(const SwapChainSupportDetails &) = default;

        [[nodiscard]] SwapChainSupportDetails(const vk::SurfaceCapabilitiesKHR &capabilities,
                const std::vector<vk::SurfaceFormatKHR> &formats,
                const std::vector<vk::PresentModeKHR> &presentModes) noexcept
            : m_capabilities(capabilities)
            , m_formats(formats)
            , m_presentModes(presentModes) {
        }

        [[nodiscard]] SwapChainSupportDetails(vk::SurfaceCapabilitiesKHR &&capabilities,
                std::vector<vk::SurfaceFormatKHR> &&formats,
                std::vector<vk::PresentModeKHR> &&presentModes) noexcept
            : m_capabilities(capabilities)
            , m_formats(std::move(formats))
            , m_presentModes(std::move(presentModes)) {
        }

        /**
         * Returns true when the object was initialised, thus the details query
         * was successful.
         */
        [[nodiscard]] inline constexpr
        operator bool() const noexcept {
            return m_capabilities.maxImageCount != 0
                && !std::empty(m_formats)
                && !std::empty(m_presentModes);
        }

        [[nodiscard]] inline constexpr const vk::SurfaceCapabilitiesKHR &
        capabilities() const noexcept {
            return m_capabilities;
        }

        [[nodiscard]] inline constexpr const std::vector<vk::SurfaceFormatKHR> &
        formats() const noexcept {
            return m_formats;
        }

        [[nodiscard]] inline constexpr const std::vector<vk::PresentModeKHR> &
        presentModes() const noexcept {
            return m_presentModes;
        }
    };

} // namespace vke
