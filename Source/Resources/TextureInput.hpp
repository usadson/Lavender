/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 - 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <memory>

#include "Source/Resources/ResourceLocation.hpp"
#include "Source/IO/Format/Image/ImageView.hpp"

namespace resources {

    enum class TextureFilter {
#define RESOURCES_ITERATE_TEXTURE_FILTERS(REGISTER_FILTER) \
        REGISTER_FILTER(LINEAR) \
        REGISTER_FILTER(NEAREST) \
        REGISTER_FILTER(CUBIC) \

#define REGISTER_FILTER(filter) filter,
        RESOURCES_ITERATE_TEXTURE_FILTERS(REGISTER_FILTER)
#undef  REGISTER_FILTER
    };

    enum class TextureCoordinateWrappingMode {
#define RESOURCES_ITERATE_TEXTURE_COORDINATE_WRAPPING_MODES(REGISTER_WRAPPING_MODE) \
        REGISTER_WRAPPING_MODE(REPEAT) \
        REGISTER_WRAPPING_MODE(REPEAT_MIRRORED) \
        \
        REGISTER_WRAPPING_MODE(CLAMP_TO_BORDER) \
        REGISTER_WRAPPING_MODE(CLAMP_TO_EDGE) \
        REGISTER_WRAPPING_MODE(CLAMP_TO_EDGE_MIRRORED) \

#define REGISTER_WRAPPING_MODE(wrappingMode) wrappingMode,
        RESOURCES_ITERATE_TEXTURE_COORDINATE_WRAPPING_MODES(REGISTER_WRAPPING_MODE)
#undef  REGISTER_WRAPPING_MODE
    };

    struct TextureSampler {
        [[nodiscard]] constexpr bool
        generateMipMaps() const noexcept {
            return m_generateMipMaps;
        }

        [[nodiscard]] constexpr TextureFilter
        magnificationFilter() const noexcept {
            return m_magnificationFilter;
        }

        [[nodiscard]] constexpr TextureFilter
        minifyingFilter() const noexcept {
            return m_minifyingFilter;
        }

        [[nodiscard]] constexpr TextureCoordinateWrappingMode
        uCoordinateWrappingMode() const noexcept {
            return m_uCoordinateWrappingMode;
        }

        [[nodiscard]] constexpr TextureCoordinateWrappingMode
        vCoordinateWrappingMode() const noexcept {
            return m_vCoordinateWrappingMode;
        }

        constexpr void
        setGenerateMipMaps(bool generate) noexcept {
            m_generateMipMaps = generate;
        }

        constexpr void
        setMagnificationFilter(TextureFilter filter) noexcept {
            m_magnificationFilter = filter;
        }

        constexpr void
        setMinifyingFilter(TextureFilter filter) noexcept {
            m_minifyingFilter = filter;
        }

        constexpr void
        setUCoordinateWrappingMode(TextureCoordinateWrappingMode wrappingMode) noexcept {
            m_uCoordinateWrappingMode = wrappingMode;
        }

        constexpr void
        setVCoordinateWrappingMode(TextureCoordinateWrappingMode wrappingMode) noexcept {
            m_vCoordinateWrappingMode = wrappingMode;
        }

    private:
        bool m_generateMipMaps{true};
        TextureFilter m_magnificationFilter{};
        TextureFilter m_minifyingFilter{};
        TextureCoordinateWrappingMode m_uCoordinateWrappingMode{};
        TextureCoordinateWrappingMode m_vCoordinateWrappingMode{};
    };

    struct TextureInput {
        [[nodiscard]] explicit constexpr
        TextureInput(io::image::ImageView imageView, TextureSampler &&sampler = {})
                : m_imageView(imageView)
                , m_sampler(std::forward<TextureSampler>(sampler)) {
        }

        TextureInput(TextureInput &&) noexcept = default;
        TextureInput(const TextureInput &) noexcept = default;
        
        [[nodiscard]] constexpr io::image::ImageView
        imageView() const noexcept {
            return m_imageView;
        }

        [[nodiscard]] constexpr TextureSampler &
        sampler() noexcept {
            return m_sampler;
        }

        [[nodiscard]] constexpr const TextureSampler &
        sampler() const noexcept {
            return m_sampler;
        }

    private:
        io::image::ImageView m_imageView;
        TextureSampler m_sampler;
    };

} // namespace resources

[[nodiscard]] constexpr std::string_view
toString(resources::TextureFilter textureFilter) noexcept {
    switch (textureFilter) {
#define CHECK_FILTER(filter) case resources::TextureFilter::filter: return #filter;
        RESOURCES_ITERATE_TEXTURE_FILTERS(CHECK_FILTER)
#undef  CHECK_FILTER
    }

    return "(invalid)";
}

[[nodiscard]] constexpr std::string_view
toString(resources::TextureCoordinateWrappingMode textureCoordinateWrappingMode) noexcept {
    switch (textureCoordinateWrappingMode) {
#define CHECK_WRAPPING_MODE(wrappingMode) case resources::TextureCoordinateWrappingMode::wrappingMode: return #wrappingMode;
        RESOURCES_ITERATE_TEXTURE_COORDINATE_WRAPPING_MODES(CHECK_WRAPPING_MODE)
#undef  CHECK_WRAPPING_MODE
    }

    return "(invalid)";
}
