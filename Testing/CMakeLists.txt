# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_executable(Tests
        Math/Matrix4x4.cpp
        ${CMAKE_SOURCE_DIR}/Source/Math/Matrix4x4.cpp
)

target_link_libraries(Tests GTest::GTest GTest::Main)
gtest_discover_tests(Tests)

