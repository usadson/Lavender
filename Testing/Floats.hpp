/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <limits>

#include <cmath>

#include "Testing/Include.hpp"

namespace Compare {

    inline void
    floats(std::string_view explanation, float a, float b, float epsilon=0.00001) {
        const auto absA = std::abs(a);
        const auto absB = std::abs(b);
        const auto diff = std::abs(a - b);

        if (a == b)
            return;

        if (a == 0 || b == 0 || (absA + absB < std::numeric_limits<float>::min())) {
            // a or b is zero or both are extremely close to it
            // relative error is less meaningful here
            EXPECT_LT(diff, epsilon * std::numeric_limits<float>::min()) << explanation;
        } else { // use relative error
            EXPECT_LT(diff / std::min((absA + absB), std::numeric_limits<float>::max()), epsilon) << explanation;
        }
    }

} // namespace Compare
