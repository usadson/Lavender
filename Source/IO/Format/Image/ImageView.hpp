/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include "Source/Base/ArrayView.hpp"
#include "Source/IO/Format/Image/ColorModel.hpp"

namespace io::image {

    struct ImageView {
        [[nodiscard]] constexpr
        ImageView(base::ArrayView<char> data, ColorModel colorModel, std::size_t width, std::size_t height)
                : m_data(data), m_colorModel(colorModel), m_width(width), m_height(height) {
        }

        [[nodiscard]] constexpr base::ArrayView<char>
        data() const noexcept {
            return m_data;
        }

        [[nodiscard]] constexpr ColorModel
        colorModel() const noexcept {
            return m_colorModel;
        }

        [[nodiscard]] constexpr std::size_t
        width() const noexcept {
            return m_width;
        }

        [[nodiscard]] constexpr std::size_t
        height() const noexcept {
            return m_height;
        }

    private:
        base::ArrayView<char> m_data;
        ColorModel m_colorModel;
        std::size_t m_width;
        std::size_t m_height;
    };
    
} // namespace io::image
