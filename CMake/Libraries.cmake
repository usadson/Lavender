# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

include(CMake/CPM.cmake)

option(USE_GLFW_SYSTEM "Use GLFW from the system libraries." ON)
option(ENABLE_VULKAN "Enable Vulkan support." ON)

set(VULKAN_VERSION "1.2" CACHE STRING "Vulkan Core Specification Revision")

if (ENABLE_VULKAN)
    add_compile_definitions(ENABLE_VULKAN)

    find_package(Vulkan REQUIRED)
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
    CPMAddPackage(
        NAME glew
        GIT_TAG "9fb23c3e61cbd2d581e33ff7d8579b572b38ee26"
        GITHUB_REPOSITORY "nigels-com/glew"
    )
endif()

if (NOT DEFINED ENV{CI})
    set(OpenGL_GL_PREFERENCE "GLVND")
    find_package(OpenGL REQUIRED)
endif()

find_package(nlohmann_json)
if (NOT nlohmann_json_FOUND)
    CPMAddPackage(
        NAME nlohmann_json
        GIT_TAG "bc889afb4c5bf1c0d8ee29ef35eaaf4c8bef8a5d"
        GITHUB_REPOSITORY "nlohmann/json"
    )
endif()

find_package(fmt)
if (NOT nlohmann_json_FOUND)
    CPMAddPackage(
        NAME fmt
        GIT_TAG "a33701196adfad74917046096bf5a2aa0ab0bb50"
        GITHUB_REPOSITORY "fmtlib/fmt"
    )
endif()

if (ENABLE_TESTING)
    find_package(GTest)
    if (NOT GTest_FOUND)
        # Build GTest at compile time
        add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty/GoogleTest)
    endif()
endif()