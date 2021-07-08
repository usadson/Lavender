/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "SwapChain.hpp"

#include <limits>

#include "Source/Vulkan/VulkanCore.hpp"
#include "Source/Window/WindowAPI.hpp"

namespace vke {

    SwapChain::~SwapChain() noexcept {
        const auto device = m_core->logicalDevice();
        const auto &allocator = m_core->allocator();

        if (!device)
            return;

        for (const auto &element : m_swapChainElements) {
            if (element.commandBuffer) {
                // TODO shouldn't this use the VulkanCore::m_allocatorCallbacks
                device.freeCommandBuffers(m_core->m_commandPool, 1, &element.commandBuffer);
            }

            if (element.frameBuffer)
                device.destroyFramebuffer(element.frameBuffer, allocator);

            if (element.imageView)
                device.destroyImageView(element.imageView, allocator);
        }

        device.destroySwapchainKHR(m_swapChain, allocator);
    }

    vk::Extent2D
    SwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) const noexcept {
        if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max()) {
            return capabilities.currentExtent;
        }

        const auto size = m_core->window()->queryFramebufferSize();

        vk::Extent2D actualExtent{size.x(), size.y()};

        actualExtent.width = std::max(capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }

    vk::PresentModeKHR
    SwapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) const noexcept {
        for (const auto &availablePresentMode : availablePresentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                // MAILBOX is the most optimal for PCs, but isn't required to
                // be available.
                return availablePresentMode;
            }
        }

        // FIFO is always available
        return vk::PresentModeKHR::eFifo;
    }

    vk::SurfaceFormatKHR
    SwapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) const noexcept {
        for (const auto &availableFormat : availableFormats) {
            if (availableFormat.format == vk::Format::eB8G8R8A8Srgb
                && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    bool SwapChain::create() {
        const auto swapChainSupport = m_core->querySwapChainSupportDetails(m_core->physicalDevice());
        if (!swapChainSupport)
            return false;

        const auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats());
        const auto presentMode = chooseSwapPresentMode(swapChainSupport.presentModes());
        const auto extent = chooseSwapExtent(swapChainSupport.capabilities());

        auto imageCount = swapChainSupport.capabilities().minImageCount + 1;

        if (swapChainSupport.capabilities().maxImageCount > 0
            && imageCount > swapChainSupport.capabilities().maxImageCount) {
            imageCount = swapChainSupport.capabilities().maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo{{}, m_core->surface(), imageCount, surfaceFormat.format, surfaceFormat.colorSpace,
            extent, 1, vk::ImageUsageFlagBits::eColorAttachment};

        const auto queueFamilySet = m_core->findQueueFamilies(m_core->physicalDevice());
        std::array queueFamilies{queueFamilySet.graphicsFamily(), queueFamilySet.presentFamily()};

        if (queueFamilySet.graphicsFamily() != queueFamilySet.presentFamily()) {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
            createInfo.queueFamilyIndexCount = static_cast<std::uint32_t>(std::size(queueFamilies));
            createInfo.pQueueFamilyIndices = std::data(queueFamilies);
        } else {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }

        createInfo.preTransform = swapChainSupport.capabilities().currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        const auto swapChain = m_core->logicalDevice().createSwapchainKHR(createInfo, m_core->allocator());
        if (swapChain.result != vk::Result::eSuccess)
            return false;

        m_swapChain = swapChain.value;

        auto swapChainImages = m_core->logicalDevice().getSwapchainImagesKHR(m_swapChain);
        if (swapChainImages.result != vk::Result::eSuccess)
            return false;

        m_swapChainElements.resize(std::size(swapChainImages.value));
        std::transform(std::cbegin(swapChainImages.value), std::cend(swapChainImages.value),
            std::begin(m_swapChainElements), [](vk::Image image) -> SwapChainElement {
                return {image, {}, {}, {}};
            }
        );

        m_swapChainExtent = extent;
        m_swapChainImageFormat = surfaceFormat.format;

        if (!createImageViews())
            return false;

        return true;
    }

    bool
    SwapChain::createFramebuffers(vk::RenderPass renderPass) noexcept {
        for (SwapChainElement &element : m_swapChainElements) {
            const std::array attachments{element.imageView};

            const vk::FramebufferCreateInfo createInfo{
                {},
                renderPass,
                static_cast<std::uint32_t>(std::size(attachments)),
                std::data(attachments),
                m_core->swapChain().extent().width,
                m_core->swapChain().extent().height,
                1
            };

            const auto creation = m_core->m_device.createFramebuffer(createInfo, m_core->allocator());

            if (creation.result != vk::Result::eSuccess) {
                return false;
            }

            element.frameBuffer = creation.value;
        }

        return true;
    }

    bool
    SwapChain::createImageViews() noexcept {
        for (SwapChainElement &element : m_swapChainElements) {
            vk::ImageViewCreateInfo createInfo{
                {},
                element.image,
                vk::ImageViewType::e2D,
                m_swapChainImageFormat,
                vk::ComponentMapping{
                    vk::ComponentSwizzle::eIdentity, // r
                    vk::ComponentSwizzle::eIdentity, // g
                    vk::ComponentSwizzle::eIdentity, // b
                    vk::ComponentSwizzle::eIdentity  // a
                },
                vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
            };

            const auto view = m_core->logicalDevice().createImageView(createInfo, m_core->allocator());
            if (view.result != vk::Result::eSuccess) {
                return false;
            }

            element.imageView = view.value;
        }

        return true;
    }

} // namespace vke
