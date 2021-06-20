/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <string_view>

#include "Source/Base/Version.hpp"

namespace base::About {

    constexpr const std::string_view applicationName{"LavenderApp"};
    constexpr const Version applicationVersion{LAVENDER_VERSION_MAJOR, LAVENDER_VERSION_MINOR, LAVENDER_VERSION_PATCH};

    constexpr const std::string_view engineName = "Lavender";
    constexpr const Version engineVersion = applicationVersion;

} // namespace base::About
