/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Source/Math/Matrix4x4.hpp"

#include <cmath>

math::Matrix4x4<float>
math::createCameraViewMatrix(math::Vector3f inForward, math::Vector3f inUp) noexcept {
    const auto forward = inForward.normalize();
    const auto right = inUp.normalize().cross(forward);
    const auto up = forward.cross(right);

    math::Matrix4x4<float> matrix{};

    matrix[0][0] = right.x();
    matrix[0][1] = right.y();
    matrix[0][2] = right.z();

    matrix[1][0] = up.x();
    matrix[1][1] = up.y();
    matrix[1][2] = up.z();

    matrix[2][0] = forward.x();
    matrix[2][1] = forward.y();
    matrix[2][2] = forward.z();

    matrix[3][3] = 1.0f;

    return matrix;
}

math::Matrix4x4<float>
math::createPerspectiveProjectionMatrix(float fov, float width, float height, float zNear, float zFar) noexcept {
    const auto tanHalfFOV = std::tan(math::toRadians(fov / 2));
    const auto aspectRatio = width / height;
    const auto zRange = zNear - zFar;

    math::Matrix4x4<float> matrix{};

    matrix[0][0] = 1 / (tanHalfFOV * aspectRatio);
    matrix[1][1] = 1 / tanHalfFOV;
    matrix[2][2] = (-zNear - zFar) / zRange;
    matrix[2][3] = 2 * zFar * zNear / zRange;
    matrix[3][2] = 1;

    return matrix;
}

