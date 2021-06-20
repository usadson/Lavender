/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Source/Vulkan/IncludeVulkan.hpp"

namespace vke {

    class Core;

    class DestroyGuard {
        Core *m_core;

    public:
        [[nodiscard]] explicit
        DestroyGuard(Core *core)
            : m_core(core) { 
        }

        DestroyGuard(DestroyGuard &&) = delete;
        DestroyGuard(const DestroyGuard &) = delete;

        ~DestroyGuard() noexcept;
    };

} // namespace vke
