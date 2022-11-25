# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
#
# Copyright (C) 2021 - 2022 Tristan Gerritsen <tristan@thewoosh.org>
# All Rights Reserved.

set(ENABLE_WIN32 ON CACHE BOOL "Enable Win32 support")
set(ENABLE_VULKAN OFF CACHE BOOL "Enable Vulkan support")

add_library(LavenderCore STATIC
            Base/Error.cpp
            Base/Task.cpp
            ECS/Node.cpp
            GraphicsAPI.cpp
            Lavender.cpp
            Lavender.hpp
            Interface/FreeCamera.cpp
            Math/Math.cpp
            Math/Matrix4x4.cpp
            Resources/FileResourceLocation.cpp
            Resources/MemoryResourceLocation.cpp
            Resources/ResourceLocateEvent.cpp
            Window/WindowAPI.cpp
)

if (ENABLE_WIN32)
    add_library(Win32Core INTERFACE)
    target_compile_definitions(LavenderCore PUBLIC LAVENDER_WIN32_SUPPORT_ENABLED)
    target_compile_definitions(Win32Core INTERFACE LAVENDER_WIN32_SUPPORT_ENABLED)
    target_link_libraries(LavenderCore PUBLIC Win32Core dwmapi)
endif()

add_executable(Lavender main.cpp)

add_subdirectory(Devices)
add_subdirectory(Event)
add_subdirectory(Input)
add_subdirectory(IO)
add_subdirectory(Network)
add_subdirectory(OpenGL)
add_subdirectory(Platform)
add_subdirectory(Window)

if (ENABLE_VULKAN)
    add_subdirectory(Vulkan)
    target_link_libraries(LavenderCore PUBLIC VulkanAPIEngine)
endif()

target_link_libraries(LavenderCore
  PRIVATE project_diagnostics
  PUBLIC  GLFWCore
          DeviceLibrary
          EventLibrary
          InputLibrary
          IOLibrary
          NetworkCore
          OpenGLAPIEngine
          PlatformCore

          GLEW::GLEW
          glfw ${GLFW_LIBRARIES}
          nlohmann_json::nlohmann_json
)

target_link_libraries(Lavender
        PRIVATE project_diagnostics
        LavenderCore
)

if (ENABLE_WIN32)
    add_executable(LavenderWin32 WinMain.cpp)
    target_link_libraries(LavenderWin32 PRIVATE project_diagnostics LavenderCore)

    if (MSVC)
        target_link_options(LavenderCore PUBLIC /PROFILE)
        target_link_options(LavenderWin32 PUBLIC /PROFILE)

        target_link_options(LavenderWin32 PRIVATE "/SUBSYSTEM:WINDOWS")
    endif()
endif()