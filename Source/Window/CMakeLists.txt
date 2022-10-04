# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright (C) 2021 - 2022 Tristan Gerritsen <tristan@thewoosh.org>
# All Rights Reserved.

add_library(GLFWCore OBJECT
            GLFWCore.cpp
            GLFWCore.hpp
)

target_link_libraries(GLFWCore
    PRIVATE project_diagnostics
            glfw)

if (ENABLE_WIN32)
    add_library(Win32CoreWindowComponent OBJECT Win32Core.cpp)
    target_compile_definitions(Win32CoreWindowComponent PUBLIC LAVENDER_WIN32_SUPPORT_ENABLED)
    target_link_libraries(Win32CoreWindowComponent PRIVATE project_diagnostics fmt::fmt)
    target_link_libraries(LavenderCore PUBLIC Win32CoreWindowComponent)
endif()
