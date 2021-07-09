/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <vector>

#include <cstdint>

namespace resources {

    enum class TextureFormat {
        RGB,
        RGBA,
    };

    struct TextureInput {
        using PixelType = float;
        using SizeType = std::uint16_t;

        TextureFormat format;
        SizeType width;
        SizeType height;
        std::vector<PixelType> pixels;

        bool optGenerateMipmap{true};
    };

} // namespace resources
