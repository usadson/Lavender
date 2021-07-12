/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Source/ECS/Entity.hpp"
#include "Source/Math/Vector.hpp"

namespace interface {

    class Camera
            : public ecs::Entity {
    public:
        [[nodiscard]] inline explicit
        Camera(math::Vector3f position = {}) noexcept
                : ecs::Entity(nullptr, {position, {}, {}}) {
        }

        [[nodiscard]] virtual math::Matrix4x4<float>
        viewMatrix() const noexcept = 0;
    };

} // namespace interface