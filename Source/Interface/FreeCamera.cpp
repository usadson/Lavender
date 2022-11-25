/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Source/Interface/FreeCamera.hpp"

#include "Source/Input/Controller.hpp"

namespace interface {

    [[nodiscard]] inline static constexpr float
    resolveDirection(bool pos, bool neg) noexcept {
        if (pos == neg)
            return 0;
        if (pos)
            return 1;
        return -1;
    }

    void
    FreeCamera::rotatePitch(float angle) noexcept {
        constexpr const math::Vector3f yAxis{0.0f, 1.0f, 0.0f};
        auto hAxis = yAxis.cross(m_forward).normalize();

        m_forward = rotateV3f(m_forward, angle, yAxis).normalize();

        m_up = m_forward.cross(hAxis).normalize();
    }

    void
    FreeCamera::rotateYaw(float angle) noexcept {
        constexpr const math::Vector3f yAxis{0.0f, 1.0f, 0.0f};
        auto hAxis = yAxis.cross(m_forward).normalize();

        m_forward = rotateV3f(m_forward, angle, hAxis).normalize();

        m_up = m_forward.cross(hAxis).normalize();
    }

    ecs::DidUpdate
    FreeCamera::onUpdate(float deltaTime) noexcept {
        if (!m_controller)
            return ecs::DidUpdate::NO;

        //deltaTime *= 20;

        bool updated{false};

        const float moveHorizontal = resolveDirection(m_controller->moveRight, m_controller->moveLeft) * deltaTime;
        transformation().translation.y() += resolveDirection(m_controller->moveUp, m_controller->moveDown) * deltaTime;
        const float moveDepth = resolveDirection(m_controller->moveForward, m_controller->moveBackward) * deltaTime;

        if (moveHorizontal != 0) {
            transformation().translation = transformation().translation.add(left().mul(moveHorizontal));
            updated = true;
        }

        if (moveDepth != 0) {
            transformation().translation = transformation().translation.add(forward().mul(moveDepth));
            updated = true;
        }

        rotatePitch(m_controller->rotatePitch * m_controller->mouseSensitivity);
        rotateYaw(m_controller->rotateYaw * m_controller->mouseSensitivity);

        if (m_controller->rotatePitch != 0 || m_controller->rotateYaw != 0)
            updated = true;

        const_cast<input::Controller *>(m_controller)->rotateYaw = 0;
        const_cast<input::Controller *>(m_controller)->rotatePitch = 0;

        return updated ? ecs::DidUpdate::YES : ecs::DidUpdate::NO;
    }

    math::Matrix4x4<float>
    FreeCamera::viewMatrix() const noexcept {
        return createCameraViewMatrix(m_forward, m_up)
                .mul(math::Matrix4x4<float>{}.translate({-position().x(), -position().y(), -position().z()}));
    }

    math::Vector3f
    FreeCamera::left() const noexcept {
        return m_up.cross(m_forward).normalize();
    }

    void
    FreeCamera::setYawAndPitch(float yaw, float pitch) noexcept {
        m_forward.x() = std::cos(pitch) * std::sin(yaw);
        m_forward.y() = -std::sin(pitch);
        m_forward.z() =  std::cos(pitch) * std::cos(yaw);
    }

} // namespace interface