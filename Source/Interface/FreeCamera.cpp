/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Source/Input/Controller.hpp"
#include "Source/Interface/FreeCamera.hpp"

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
    FreeCamera::onUpdate(float deltaTime) noexcept {
        if (!m_controller)
            return;

        const float moveX = resolveDirection(m_controller->moveRight, m_controller->moveLeft) * deltaTime;
        const float moveY = resolveDirection(m_controller->moveUp, m_controller->moveDown) * deltaTime;
        const float moveZ = resolveDirection(m_controller->moveForward, m_controller->moveBackward) * deltaTime;

        if (moveZ != 0) {
            transformation().translation.x() += std::sin(math::toRadians(transformation().rotation.y())) * -1.0f * moveZ;
            transformation().translation.z() += std::cos(math::toRadians(transformation().rotation.y())) * moveZ;
        }
        if (moveX != 0) {
            transformation().translation.x() += std::sin(math::toRadians(transformation().rotation.y() - 90)) * -1.0f * moveX;
            transformation().translation.z() += std::cos(math::toRadians(transformation().rotation.y() - 90)) * moveX;
        }

        transformation().translation.y() += moveY;
    }

    math::Matrix4x4<float>
    FreeCamera::viewMatrix() const noexcept {
        return math::Matrix4x4<float>{}
                   .identity()
                   .rotate({math::toRadians(transformation().rotation.x()), 0.0f, 0.0f})
                   .rotate({0.0f, math::toRadians(transformation().rotation.y()), 0.0f})
                   .translate({-transformation().translation.x(), -transformation().translation.y(), -transformation().translation.z()});
    }

} // namespace interface