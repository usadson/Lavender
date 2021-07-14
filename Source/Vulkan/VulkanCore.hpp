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

#pragma once

#include "Source/GraphicsAPI.hpp"

#include <compare>
#include <vector>

#include "Source/Vulkan/DestroyGuard.hpp"
#include "Source/Vulkan/IncludeVulkan.hpp"
#include "Source/Vulkan/Pipeline.hpp"
#include "Source/Vulkan/QueueFamilySet.hpp"
#include "Source/Vulkan/SwapChain.hpp"
#include "Source/Vulkan/SwapChainSupportDetails.hpp"

namespace vke {

    class Core
            : public GraphicsAPI {
        WindowAPI *m_windowAPI{nullptr};

        friend class DestroyGuard;
        friend class SwapChain;

        /**
         * The destroy guard makes sure the following objects get destructed
         * automatically, in this order:
         * 
         * - m_commandPool
         * - m_device
         * - m_surface
         * - m_debugUtilsMessenger
         * - m_instance
         * 
         * Make sure this declaration doesn't get moved, since it messes with
         * the destruction order.
         */
        DestroyGuard m_destroyGuard{this};

        vk::Instance m_instance{};
        vk::Optional<const vk::AllocationCallbacks> m_allocatorCallbacks{nullptr};
        vk::DebugUtilsMessengerEXT m_debugUtilsMessenger{};
        vk::PhysicalDevice m_physicalDevice{};
        vk::Device m_device{};
        vk::Queue m_graphicsQueue{};
        vk::Queue m_presentQueue{};
        vk::SurfaceKHR m_surface{};
        vk::CommandPool m_commandPool{};

        SwapChain m_swapChain{this};
        Pipeline m_pipeline{this};

        [[nodiscard]] bool
        checkDeviceExtensionSupport(vk::PhysicalDevice device) const noexcept;

        /**
         * Checks the available layers of this Vulkan implementation whether it
         * has at least the layers as specified in g_validationLayers
         * (VulkanCore.cpp).
         * 
         * Validation layers is an infrastructure for Vulkan implementation
         * vendors to signal applicationd developers in debugging about e.g.
         * errornous API usage. Instead of having to rely on vk::Result to
         * figure out what went wrong, the validation layer can report a string
         * message, explaining what parameters were faulty.
         * 
         * The above is a mere example, as validation layers can be used for
         * many more things.
         */
        [[nodiscard]] bool
        checkValidationLayerSupport();

        /**
         * In the instance creating process, some extensions are required to exist in
         * order to interface with the WindowAPI or use debugging tools.
         */
        [[nodiscard]] std::vector<const char *>
        collectRequiredInstanceExtensions(WindowAPI *) const;

        [[nodiscard]] bool
        createCommandPool() noexcept;

        [[nodiscard]] bool
        createFramebuffers() noexcept;

        [[nodiscard]] bool
        createGraphicsPipeline() noexcept;

        /**
         * Creates the Vulkan "context". Most Vulkan APIs are only accessible
         * through this instance.
         */
        [[nodiscard]] bool
        createInstance(WindowAPI *);

        [[nodiscard]] bool
        createLogicalDevice();

        [[nodiscard]] bool
        createSurface(WindowAPI *) noexcept;

        [[nodiscard]] bool
        createSwapChain() noexcept;

        [[nodiscard]] QueueFamilySet
        findQueueFamilies(vk::PhysicalDevice device) const noexcept;

        /**
         * Picks a physical device. This initialises m_physicalDevice.
         */
        [[nodiscard]] bool
        pickPhysicalDevice();

        /**
         * Produces the information to create the debug utils messenger. This
         * is given a separate function, because it is used in createInstance()
         * and setupDebugMessenger().
         */
        [[nodiscard]] vk::DebugUtilsMessengerCreateInfoEXT
        produceUtilsMessengerCreateInfo() const noexcept;

        [[nodiscard]] SwapChainSupportDetails
        querySwapChainSupportDetails(vk::PhysicalDevice physicalDevice) const noexcept;
            
        /**
         * In the physical device selection process, devices are weighed on their
         * suitability in this program.
         * 
         * Note: a return value of 0 means the device is unsuitable.
         */
        [[nodiscard]] std::size_t
        rateDeviceSuitability(vk::PhysicalDevice device) const noexcept;

        /**
         * Sets up the debug messenger. This initialises m_physicalDevice. The debug
         * messenger is an interface for the Vulkan implementation to report possible
         * errors, emit warnings, give suggestions, etc.
         */
        [[nodiscard]] bool
        setupDebugMessenger();

    public:
        [[nodiscard]] inline explicit
        Core(const ecs::EntityList *entityList,
             const input::Controller *controller,
             const interface::Camera *camera) noexcept
                : GraphicsAPI(entityList, controller, camera) {
        }

        [[nodiscard]] bool
        initialize(WindowAPI *) override;

        [[nodiscard]] inline resources::ModelGeometryDescriptor *
        createModelGeometry(const resources::ModelGeometry &) noexcept override {
            return {};
        }

        [[nodiscard]] inline resources::TextureDescriptor *
        createTexture(const resources::TextureInput &) noexcept override {
            return {};
        }

        inline void
        renderEntities() noexcept override {
        }

        [[nodiscard]] inline decltype(auto)
        allocator() const noexcept {
            return m_allocatorCallbacks;
        }

        [[nodiscard]] inline constexpr vk::Instance
        instance() const noexcept {
            return m_instance;
        }

        [[nodiscard]] inline constexpr vk::Device
        logicalDevice() const noexcept {
            return m_device;
        }

        [[nodiscard]] inline constexpr vk::PhysicalDevice
        physicalDevice() const noexcept {
            return m_physicalDevice;
        }

        [[nodiscard]] inline constexpr vk::SurfaceKHR
        surface() const noexcept {
            return m_surface;
        }

        [[nodiscard]] inline constexpr SwapChain &
        swapChain() noexcept {
            return m_swapChain;
        }

        [[nodiscard]] inline constexpr const SwapChain &
        swapChain() const noexcept {
            return m_swapChain;
        }

        [[nodiscard]] inline const resources::ModelDescriptor *
        uploadModelDescriptor(resources::ModelDescriptor &&) noexcept override {
            return {};
        }

        [[nodiscard]] inline constexpr WindowAPI *
        window() noexcept {
            return m_windowAPI;
        }

        [[nodiscard]] inline constexpr const WindowAPI *
        window() const noexcept {
            return m_windowAPI;
        }
    };

} // namespace vke
