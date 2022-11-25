# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

include(CMake/CPM.cmake)

option(USE_GLFW_SYSTEM "Use GLFW from the system libraries." ON)
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
if (USE_GLFW_SYSTEM)
    find_package(glfw3)
else()
    add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty/glfw)
    include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/glfw/include)
endif()

find_package(GLEW)
if (NOT GLEW_FOUND)
    CPMAddPackage("gh:nigels-com/glew")
endif()

set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

find_package(nlohmann_json)
if (NOT nlohmann_json_FOUND)
    CPMAddPackage("gh:nlohmann/json")
endif()

find_package(fmt)
if (NOT nlohmann_json_FOUND)
    CPMAddPackage("gh:fmtlib/fmt")
endif()

CPMAddPackage("gh:zeux/pugixml@1.13")

if (ENABLE_TESTING)
    find_package(GTest)
    if (NOT GTest_FOUND)
        # Build GTest at compile time
        add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty/GoogleTest)
    endif()
endif()