# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# OpenGL® and the oval logo are trademarks or registered trademarks
# of Hewlett Packard Enterprise in the United States and/or other
# countries worldwide.

add_library(OpenGLAPIEngine OBJECT
        DebugMessenger.cpp
        GLCore.cpp
        GraphicsHandle.hpp
        Resources/Shader.cpp
        Resources/ShaderProgram.cpp
)

target_link_libraries(OpenGLAPIEngine
  PRIVATE project_diagnostics
  PUBLIC GLEW::GLEW
         OpenGL::GL)

add_dependencies(OpenGLAPIEngine SPIRV_Shaders)
