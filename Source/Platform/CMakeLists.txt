# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
# All Rights Reserved.

add_library(PlatformCore OBJECT)
target_link_libraries(PlatformCore PRIVATE project_diagnostics fmt::fmt)

if (ENABLE_WIN32)
    target_sources(PlatformCore PRIVATE
            WinPlatform.cpp
            Win32/ConsoleManager.cpp
            Win32/Input.cpp
            Win32/UndocumentedAPIs.cpp
            Win32/UWP.cpp
    )
    if (MSVC)
        target_compile_options(PlatformCore PRIVATE /await)
    endif()
    target_link_libraries(PlatformCore PRIVATE onecore)
    set_target_properties(PlatformCore PROPERTIES VS_WINRT_COMPONENT TRUE)
    target_compile_definitions(PlatformCore PUBLIC LAVENDER_WIN32_SUPPORT_ENABLED)
endif()
