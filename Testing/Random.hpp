/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <limits> // for std::numeric_limits
#include <random>

#include <cmath> // for std::nextafter
#include <cstdint>

#define RANDOM_FOREACH() for (std::size_t randomForeachIndex = 0; randomForeachIndex < 100; ++randomForeachIndex)

namespace Random {

    static std::random_device device{};
    std::uniform_int_distribution<std::size_t> sizeDistribution(2, 20);

    [[nodiscard]] inline float
    getFloat(float min, float max) {
        static std::default_random_engine engine;
        std::uniform_real_distribution<float> distrib(
            min, std::nextafter(max, std::numeric_limits<float>::max()));
        return static_cast<float>(distrib(engine));
    }

    [[nodiscard]] inline float
    getFloat() {
        return getFloat(std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
    }

} // namespace Random

