/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <vector>

namespace resources {

    struct ModelGeometry {
        using IndexType = unsigned int;
        using TextureCoordType = float;
        using VertexType = float;

        std::vector<IndexType> indices{};
        std::vector<TextureCoordType> textureCoordinates{};
        std::vector<VertexType> vertices{};
    };

} // namespace resources
