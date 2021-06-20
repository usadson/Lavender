# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Vulkan and the Vulkan logo are registered trademarks of the Khronos
# Group Inc.
#
# OpenGL® and the oval logo are trademarks or registered trademarks of
# Hewlett Packard Enterprise in the United States and/or other countries
# worldwide.

set(GLSL_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/shader.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shader.frag"
)

set(GLSL_COMPILER ${Vulkan_GLSLC_EXECUTABLE})

message(TARGET_INFO "Vulkan version: ${VULKAN_VERSION}")

if (${VULKAN_VERSION} STREQUAL "1.2")
    set(GLSL_CLIENT vulkan1.2)
elseif (${VULKAN_VERSION} STREQUAL "1.1")
    set(GLSL_CLIENT vulkan1.1)
else()
    set(GLSL_CLIENT vulkan1.0)
endif()

set(GLSL_OPTIONS
    -Werror # Treat warnings as errors
    -O      # Optimize for performance
    --target-env=${GLSL_CLIENT}
)

# We might want to support glslc AND glslangValidator in the future:
# find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_BINARY_DIR}/Resources/Shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/Resources/Shaders/"
    COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL_OPTIONS} ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(SPIRV_Shaders DEPENDS ${SPIRV_BINARY_FILES})
