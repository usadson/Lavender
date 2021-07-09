/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Source/Resources/GraphicsHandleBase.hpp"

namespace resources {

    class ModelDescriptor {
        GraphicsHandleBase *m_graphicsHandle;

    public:
        [[nodiscard]] inline constexpr explicit
        ModelDescriptor(GraphicsHandleBase *graphicsHandle) noexcept
                : m_graphicsHandle(graphicsHandle) {
        }

        [[nodiscard]] inline constexpr GraphicsHandleBase *
        graphicsBase() noexcept {
            return m_graphicsHandle;
        }

        [[nodiscard]] inline constexpr const GraphicsHandleBase *
        graphicsBase() const noexcept {
            return m_graphicsHandle;
        }
    };

} // namespace resources
