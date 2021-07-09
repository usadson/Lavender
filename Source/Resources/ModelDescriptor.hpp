/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Source/Resources/ModelGeometryDescriptor.hpp"

namespace resources {

    class ModelDescriptor {
        ModelGeometryDescriptor *m_geometryDescriptor;

    public:
        [[nodiscard]] inline constexpr explicit
        ModelDescriptor(ModelGeometryDescriptor *geometryDescriptor) noexcept
                : m_geometryDescriptor(geometryDescriptor) {
        }

        [[nodiscard]] inline constexpr ModelGeometryDescriptor *
        geometryDescriptor() noexcept {
            return m_geometryDescriptor;
        }

        [[nodiscard]] inline constexpr const ModelGeometryDescriptor *
        geometryDescriptor() const noexcept {
            return m_geometryDescriptor;
        }
    };

} // namespace resources
