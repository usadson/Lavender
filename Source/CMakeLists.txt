# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_library(LavenderCore OBJECT
  "GraphicsAPI.hpp"
  "Lavender.cpp"
  "Lavender.hpp")

add_executable(Lavender main.cpp)

add_subdirectory(IO)
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
          VulkanAPIEngine
)
