# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

option(ENABLE_VULKAN "Enable Vulkan support." ON)

set(VULKAN_VERSION "1.2" CACHE STRING "Vulkan Core Specification Revision")

if (ENABLE_VULKAN)
	add_compile_definitions(ENABLE_VULKAN)

	find_package(Vulkan REQUIRED FATAL_ERROR)
	include_directories(${Vulkan_INCLUDE_DIRS})
endif()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty/glfw)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/glfw/include)
