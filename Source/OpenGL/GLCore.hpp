/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Vulkan and the Vulkan logo are registered trademarks of the Khronos Group
 * Inc.
 *
 * vke is an abbreviation for VulKan Engine.
 */

#pragma once

#include <compare>
#include <vector>

#include "Source/GraphicsAPI.hpp"

namespace gle {

    class Core
            : public GraphicsAPI {
        WindowAPI *m_windowAPI{nullptr};

    public:
        [[nodiscard]] bool
        initialize(WindowAPI *) override;
    };

} // namespace gle
