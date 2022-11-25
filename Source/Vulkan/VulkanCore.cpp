/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "VulkanCore.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <limits>
#include <map>
#include <set>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "Source/Base/About.hpp"
#include "Source/Vulkan/CustomDispatch.hpp"
#include "Source/Window/WindowAPI.hpp"

namespace vke {

    constexpr const bool opt_enableValidationLayers{
#ifdef NDEBUG
        false
#else
        true
#endif
    };

    constexpr base::FunctionErrorGenerator errors{"VulkanCore", "Core"};

    const std::array g_validationLayers{"VK_LAYER_KHRONOS_validation"};
    const std::array g_deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    [[nodiscard]] static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                  void *pUserData) {
            std::printf("vke: validation layer: (%u %u)\"%s\"\n\n", static_cast<std::uint32_t>(messageSeverity),
                messageType, pCallbackData->pMessage);

        static_cast<void>(messageSeverity);
        static_cast<void>(messageType);
        static_cast<void>(pUserData);

        return VK_FALSE;
    }

    base::Error
    Core::initialize(WindowAPI *window) {
        m_windowAPI = window;

#define ADD_TASK(name, function) if (!function) {\
            return errors.error("Invoke task " name, "Failed (TODO)"); \
        }

#ifndef NDEBUG
        ADD_TASK("check-validation-layer-support", (!opt_enableValidationLayers || checkValidationLayerSupport()))
#endif
        ADD_TASK("create-instance", createInstance(window))
        ADD_TASK("setup-debug-messenger", setupDebugMessenger())
        ADD_TASK("create-surface", createSurface(window))
        ADD_TASK("pick-physical-device", pickPhysicalDevice())
        ADD_TASK("create-logical-device", createLogicalDevice())
        ADD_TASK("create-swap-chain", createSwapChain())
        ADD_TASK("create-graphics-pipeline", createGraphicsPipeline())
        ADD_TASK("create-frame-buffers", createFramebuffers())
        ADD_TASK("create-command-pool", createCommandPool())

        return base::Error::success();
#undef ADD_TASK
    }

    bool
    Core::checkDeviceExtensionSupport(vk::PhysicalDevice device) const noexcept {
        const auto result = device.enumerateDeviceExtensionProperties();
        if (result.result != vk::Result::eSuccess) {
            return false;
        }

        const auto &availableExtensions = result.value;
        std::set<std::string> requiredExtensions{std::cbegin(g_deviceExtensions), std::cend(g_deviceExtensions)};

        for (const auto &extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return std::empty(requiredExtensions);
    }

    bool
    Core::checkValidationLayerSupport() {
        const auto properties = vk::enumerateInstanceLayerProperties();

        if (properties.result != vk::Result::eSuccess) {
            return false;
        }

        const auto &availableLayers = properties.value;

        for (const char *layerName : g_validationLayers) {
            if (!std::any_of(std::cbegin(availableLayers), std::cend(availableLayers),
                [layerName](const VkLayerProperties &prop) {
                    return std::strcmp(layerName, prop.layerName) == 0;
                })
            ) {
                std::printf("vke: couldn't find validation layer: \"%s\"\n", layerName);
                std::printf("vke: available layers(%zu):\n", std::size(availableLayers));
                for (std::size_t i = 0; i < std::size(availableLayers); ++i) {
                    std::printf("(%zu) %s (version %u spec %u)\n    %s\n\n",
                        i + 1,
                        std::data(availableLayers[i].layerName),
                        availableLayers[i].implementationVersion,
                        availableLayers[i].specVersion,
                        std::data(availableLayers[i].description));
                }

                return false;
            }
        }

        return true;
    }

    std::vector<const char *>
    Core::collectRequiredInstanceExtensions(WindowAPI *window) const {
        const auto array = window->getRequiredVulkanInstanceExtensions();

        std::vector<const char *> extensions(std::cbegin(array), std::cend(array));

        if (opt_enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool
    Core::createCommandPool() noexcept {
        const auto queueFamilies = findQueueFamilies(m_physicalDevice);

        const vk::CommandPoolCreateInfo createInfo{{}, queueFamilies.graphicsFamily()};

        const auto creation = m_device.createCommandPool(createInfo, m_allocatorCallbacks);
        m_commandPool = creation.value;

        return creation.result == vk::Result::eSuccess;
    }

    bool
    Core::createFramebuffers() noexcept {
        return m_swapChain.createFramebuffers(m_pipeline.renderPass());
    }

    bool
    Core::createGraphicsPipeline() noexcept {
        return m_pipeline.create();
    }

    bool
    Core::createInstance(WindowAPI *window) {
        constexpr const vk::ApplicationInfo applicationInfo{
            base::About::applicationName.data(),
            base::About::applicationVersion.asCombined(),
            base::About::engineName.data(),
            base::About::engineVersion.asCombined(),
            VK_API_VERSION_1_2
        };

        vk::InstanceCreateInfo createInfo{};
        createInfo.pApplicationInfo = &applicationInfo;

        const auto extensions = collectRequiredInstanceExtensions(window);
        createInfo.ppEnabledExtensionNames = std::data(extensions);
        createInfo.enabledExtensionCount = static_cast<std::uint32_t>(std::size(extensions));

        const auto debugMessengerCreateInfo = produceUtilsMessengerCreateInfo();

        if (opt_enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<std::uint32_t>(std::size(g_validationLayers));
            createInfo.ppEnabledLayerNames = std::data(g_validationLayers);
            createInfo.pNext = &debugMessengerCreateInfo;
        }

        const auto status = vk::createInstance(&createInfo, m_allocatorCallbacks, &m_instance);

        if (status == vk::Result::eSuccess)
            return true;

        std::printf("vke: failed to create instance! result: %zu\n",
            static_cast<std::size_t>(status));

        return false;
    }

    bool
    Core::createLogicalDevice() {
        const auto queueFamilySet = findQueueFamilies(m_physicalDevice);

        const float queuePriority{1.0};

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{};

        const std::set<std::uint32_t> uniqueFamilyIndices{
            queueFamilySet.graphicsFamily(),
            queueFamilySet.presentFamily(),
        };

        for (const auto &uniqueFamilyIndex : uniqueFamilyIndices) {
            queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags{}, uniqueFamilyIndex, 1, &queuePriority);
        }

        vk::PhysicalDeviceFeatures enabledFeatures{};

        vk::DeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.pQueueCreateInfos = std::data(queueCreateInfos);
        deviceCreateInfo.queueCreateInfoCount = static_cast<std::uint32_t>(std::size(queueCreateInfos));

        deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

        deviceCreateInfo.enabledExtensionCount = 0;

        if (opt_enableValidationLayers) {
            deviceCreateInfo.enabledLayerCount = static_cast<std::uint32_t>(std::size(g_validationLayers));
            deviceCreateInfo.ppEnabledLayerNames = std::data(g_validationLayers);
        }

        deviceCreateInfo.enabledExtensionCount = static_cast<std::uint32_t>(std::size(g_deviceExtensions));
        deviceCreateInfo.ppEnabledExtensionNames = std::data(g_deviceExtensions);

        const auto [result, device] = m_physicalDevice.createDevice(deviceCreateInfo, m_allocatorCallbacks, {});
        if (result != vk::Result::eSuccess) {
            return false;
        }
        
        m_device = device;
        device.getQueue(queueFamilySet.graphicsFamily(), 0, &m_graphicsQueue);
        device.getQueue(queueFamilySet.presentFamily(), 0, &m_presentQueue);

        return true;
    }

    bool
    Core::createSurface(WindowAPI *window) noexcept {
        return window->createVulkanSurface(m_instance, m_allocatorCallbacks, &m_surface);
    }

    bool
    Core::createSwapChain() noexcept {
        return m_swapChain.create();
    }

    QueueFamilySet
    Core::findQueueFamilies(vk::PhysicalDevice device) const noexcept {
        QueueFamilySet set{};

        const auto queueFamilies = device.getQueueFamilyProperties();
        assert(std::size(queueFamilies) < std::numeric_limits<std::uint32_t>::max());

        for (std::uint32_t i = 0; i < std::size(queueFamilies); ++i) {
            const auto &family = queueFamilies[i];

            if (family.queueFlags & vk::QueueFlagBits::eGraphics) {
                set.setGraphicsFamily(i);
            }

            const auto surfaceSupport = device.getSurfaceSupportKHR(i, m_surface);

            if (surfaceSupport.result == vk::Result::eSuccess
                    && surfaceSupport.value == VK_TRUE) {
                set.setPresentFamily(i);
            }

            if (set.isComplete()) {
                return set;
            }
        }

        return set;
    }

    bool
    Core::pickPhysicalDevice() {
        const auto devices = m_instance.enumeratePhysicalDevices();
        
        if (devices.result != vk::Result::eSuccess)
            return false;

        std::multimap<std::size_t, vk::PhysicalDevice> candidates;

        for (const auto &device : devices.value) {
            const auto score = rateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.rbegin()->first == 0) {
            // no device is suitable.
            return false;
        }

        m_physicalDevice = candidates.rbegin()->second;
        return true;
    }

    vk::DebugUtilsMessengerCreateInfoEXT
    Core::produceUtilsMessengerCreateInfo() const noexcept {
        return {
            {},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
                | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
            debugCallback
        };
    }

    SwapChainSupportDetails
    Core::querySwapChainSupportDetails(vk::PhysicalDevice physicalDevice) const noexcept {
        auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
        if (capabilities.result != vk::Result::eSuccess)
            return {};

        auto formats = physicalDevice.getSurfaceFormatsKHR(m_surface);
        if (formats.result != vk::Result::eSuccess)
            return {};

        auto presentModes = physicalDevice.getSurfacePresentModesKHR(m_surface);
        if (presentModes.result != vk::Result::eSuccess)
            return {};

        return {std::move(capabilities.value), std::move(formats.value), std::move(presentModes.value)};
    }

    std::size_t
    Core::rateDeviceSuitability(vk::PhysicalDevice physicalDevice) const noexcept {
        if (!checkDeviceExtensionSupport(physicalDevice)) {
            return 0;
        }

        const auto swapChainSupport = querySwapChainSupportDetails(physicalDevice);
        if (!swapChainSupport) {
            return 0;
        }
            
        const auto familySet = findQueueFamilies(physicalDevice);
        if (!familySet.isComplete()) {
            std::printf("vke: skipping device because of incomplete QueueFamilySet\n");
            return 0;
        }

        const auto properties = physicalDevice.getProperties();
        std::printf("vke: physical device \"%s\"\n", properties.deviceName.data());
        std::size_t score{};

        // Discrete GPUs have a significant performance advantage
        if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += properties.limits.maxImageDimension2D;

        return score;
    }

    bool
    Core::setupDebugMessenger() {
        const auto createInfo = produceUtilsMessengerCreateInfo();

        auto [result, messenger] = m_instance.createDebugUtilsMessengerEXT<vke::CustomDispatch>(
                 createInfo, m_allocatorCallbacks, {});

        if (result != vk::Result::eSuccess) {
            std::printf("vke: failed to create debug messenger! result: %zu\n",
                static_cast<std::size_t>(result));
            return false;
        }

        m_debugUtilsMessenger = std::move(messenger);

        return true;
    }

} // namespace vke
