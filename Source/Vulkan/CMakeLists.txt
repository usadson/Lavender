# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Vulkan and the Vulkan logo are registered trademarks of the Khronos
# Group Inc.

add_library(VulkanAPIEngine OBJECT
        DestroyGuard.hpp
        Pipeline.cpp
        SwapChain.cpp
        VulkanCore.cpp)

target_link_libraries(VulkanAPIEngine
  PRIVATE project_diagnostics
  PUBLIC  Vulkan::Vulkan ${Vulkan_LIBRARIES})

add_dependencies(VulkanAPIEngine SPIRV_Shaders)
