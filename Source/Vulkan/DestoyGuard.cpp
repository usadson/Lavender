/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "DestroyGuard.hpp"

#include "Source/Vulkan/CustomDispatch.hpp"
#include "Source/Vulkan/VulkanCore.hpp"

namespace vke {

    DestroyGuard::~DestroyGuard() noexcept {
        if (!m_core->m_instance)
            return;

        const auto &allocator = m_core->allocator();
        const auto &instance = m_core->m_instance;

        if (m_core->m_commandPool)
            m_core->m_device.destroyCommandPool(m_core->m_commandPool, allocator);

        if (m_core->m_device)
            m_core->m_device.destroy(allocator);

        if (m_core->m_surface)
            instance.destroySurfaceKHR(m_core->m_surface, allocator);

        if (m_core->m_debugUtilsMessenger)
            instance.destroyDebugUtilsMessengerEXT<vke::CustomDispatch>(m_core->m_debugUtilsMessenger, allocator, {});

        instance.destroy();
    }
    
} // namespace vke
