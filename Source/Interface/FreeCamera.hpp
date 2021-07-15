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

        math::Vector3f m_forward{0.0f, 0.0f, 1.0f};
        math::Vector3f m_up{0.0f, 1.0f, 0.0f};

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

        [[nodiscard]] inline constexpr math::Vector3f &
        forward() noexcept {
            return m_forward;
        }

        [[nodiscard]] inline math::Vector3f
        position() const noexcept {
            return transformation().translation;
        }

        [[nodiscard]] inline constexpr math::Vector3f
        up() const noexcept {
            return m_up;
        }

        [[nodiscard]] math::Vector3f
        left() const noexcept;

        void
        rotatePitch(float angle) noexcept;

        void
        rotateYaw(float angle) noexcept;

        void
        setYawAndPitch(float yaw, float pitch) noexcept override;
    };

} // namespace interface