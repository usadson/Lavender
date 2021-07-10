/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Source/ECS/Forward.hpp"
#include "Source/Math/Transformation.hpp"
#include "Source/Resources/ModelDescriptor.hpp"

namespace ecs {

    class Entity {
        const resources::ModelDescriptor *m_modelDescriptor{};
        math::Transformation m_transformation{};

    public:
        [[nodiscard]] inline constexpr explicit
        Entity(const resources::ModelDescriptor *modelDescriptor, math::Transformation transformation={}) noexcept
                : m_modelDescriptor(modelDescriptor)
                , m_transformation(transformation) {
        }

        [[nodiscard]] inline const resources::ModelDescriptor *
        modelDescriptor() const noexcept {
            return m_modelDescriptor;
        }

        [[nodiscard]] inline math::Transformation &
        transformation() noexcept {
            return m_transformation;
        }

        [[nodiscard]] inline const math::Transformation &
        transformation() const noexcept {
            return m_transformation;
        }
    };

} // namespace ecs