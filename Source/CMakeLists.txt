# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_library(LavenderCore OBJECT
            GraphicsAPI.hpp
            Lavender.cpp
            Lavender.hpp
            Interface/FreeCamera.cpp
            Math/Math.cpp
            Math/Matrix4x4.cpp
)

add_executable(Lavender main.cpp)

add_subdirectory(IO)
add_subdirectory(OpenGL)
add_subdirectory(Vulkan)
add_subdirectory(Window)

target_link_libraries(LavenderCore
  PRIVATE project_diagnostics
  PUBLIC  glfw ${GLFW_LIBRARIES}
)

target_link_libraries(Lavender
  PRIVATE project_diagnostics
          GLFWCore
          IOLibrary
          LavenderCore
          OpenGLAPIEngine
          VulkanAPIEngine
          nlohmann_json::nlohmann_json
)
