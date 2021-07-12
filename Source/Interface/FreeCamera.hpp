/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Source/Input/Forward.hpp"
#include "Source/Interface/Camera.hpp"
#include "Source/Math/Vector.hpp"

namespace interface {

    class FreeCamera
            : public Camera {
        const input::Controller *m_controller;

    public:
        [[nodiscard]] inline explicit
        FreeCamera(const input::Controller *controller, math::Vector3f position = {}) noexcept
                : Camera(position)
                , m_controller(controller) {
        }

        void
        onUpdate(float deltaTime) noexcept override;

        [[nodiscard]] math::Matrix4x4<float>
        viewMatrix() const noexcept override;
    };

} // namespace interface