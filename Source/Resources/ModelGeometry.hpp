/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <vector>

namespace resources {

    struct ModelGeometry {
        using VertexType = float;
        using IndexType = unsigned int;

        std::vector<VertexType> vertices{};
        std::vector<IndexType> indices{};
    };

} // namespace resources
