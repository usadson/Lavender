# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

option(ENABLE_PCH "Enable PreCompiledHeaders" ON)

if (ENABLE_PCH)
    add_library(PCH_Libraries INTERFACE)
    target_link_libraries(PCH_Libraries INTERFACE GLEW::GLEW)
    target_precompile_headers(PCH_Libraries INTERFACE
            # C++ standard libraries
            <chrono>
            <memory>
            <string>
            <vector>

            # Libraries
            <GL/glew.h>
    )

    add_library(PCH_Vulkan INTERFACE)
    if (ENABLE_VULKAN)
        target_link_libraries(PCH_Vulkan INTERFACE Vulkan::Vulkan)
        target_precompile_headers(PCH_Vulkan INTERFACE
                "Source/Vulkan/IncludeVulkan.hpp")
    endif()

    target_link_libraries(project_diagnostics INTERFACE PCH_Libraries PCH_Vulkan)
endif()
