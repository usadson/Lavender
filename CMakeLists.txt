# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.8)

include(CMake/Version.cmake)

project(Lavender
        HOMEPAGE_URL https://gitlab.com/thewoosh/lavender
        VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(.)
include_directories(Source)

add_library(project_diagnostics INTERFACE)
include(CMake/CompilerDiagnostics.cmake)
set_project_diagnostics(project_diagnostics)

include(CMake/Libraries.cmake)

add_subdirectory(Resources)
add_subdirectory(Source)
