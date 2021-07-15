/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Testing/Floats.hpp"
#include "Testing/Include.hpp"
#include "Testing/Random.hpp"

#include "Source/Math/Matrix4x4.hpp"

TEST(Math_Matrix4x4, Identity) {
    math::Matrix4x4<float> matrix{};
    matrix.identity();

    for (std::uint8_t i = 0; i < 4; ++i) {
        for (std::uint8_t j = 0; j < 4; ++j) {
            if (i == j) {
                EXPECT_EQ(matrix[i][j], 1.0f);
            } else {
                EXPECT_EQ(matrix[i][j], 0.0f);
            }
        }
    }
}

TEST(Math_Matrix4x4, Translation) {
#undef EXCLAIMER
#define EXCLAIMER "i=" << static_cast<unsigned>(i) << " j=" << static_cast<unsigned>(j)

    RANDOM_FOREACH() {
        const math::Vector3f translation{Random::getFloat(), Random::getFloat(), Random::getFloat()};

        math::Matrix4x4<float> matrix{};
        matrix.translate(translation);

        for (std::uint8_t i = 0; i < 4; ++i) {
            for (std::uint8_t j = 0; j < 4; ++j) {
                if (i == j) {
                    EXPECT_EQ(matrix[i][j], 1.0f) << EXCLAIMER;
                } else if (j == 3) {
                    switch (i) {
                    case 0: EXPECT_EQ(matrix[i][j], translation.x()); break;
                    case 1: EXPECT_EQ(matrix[i][j], translation.y()); break;
                    case 2: EXPECT_EQ(matrix[i][j], translation.z()); break;
                    default: std::abort();
                    }
                } else {
                    EXPECT_EQ(matrix[i][j], 0.0f) << EXCLAIMER;
                }
            }
        }
    }
}

TEST(Math_Matrix4x4, CameraViewMatrixPredetermined) {
    constexpr const math::Vector3f forward{-0.801153f, 0.000000f, -0.598460f};
    constexpr const math::Vector3f up{0.0f, 1.0f, 0.0f};
    const auto matrix = math::createCameraViewMatrix(forward, up);

    const auto right = up.normalizeCopy().cross(forward);

//    for (std::size_t i = 0; i < 4; ++i)
//        std::printf("%f %f %f %f\n",
//            static_cast<float>(matrix[i][0]), static_cast<float>(matrix[i][1]),
//            static_cast<float>(matrix[i][2]), static_cast<float>(matrix[i][3]));

    Compare::floats("right-x", matrix[0][0], right.x());
    Compare::floats("right-y", matrix[0][1], right.y());
    Compare::floats("right-z", matrix[0][2], right.z());

    Compare::floats("up-x", matrix[1][0], up.x());
    Compare::floats("up-y", matrix[1][1], up.y());
    Compare::floats("up-z", matrix[1][2], up.z());

    Compare::floats("forward-x", matrix[2][0], forward.x());
    Compare::floats("forward-y", matrix[2][1], forward.y());
    Compare::floats("forward-z", matrix[2][2], forward.z());
}
