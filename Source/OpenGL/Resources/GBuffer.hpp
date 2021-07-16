/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#pragma once

#include <cstddef> // for std::size_t

#include "Source/OpenGL/Types.hpp"

namespace gle {

    class GBuffer {
        GL::FramebufferType m_buffer{};

        GL::TextureType m_position{};
        GL::TextureType m_normal{};
        GL::TextureType m_colorSpec{};

        GL::TextureType m_depthBuffer{};

        GL::SizeI m_width{};
        GL::SizeI m_height{};

        [[nodiscard]] bool
        createDepthBuffer() noexcept;

        [[nodiscard]] bool
        createTexture(GL::TextureType *destination, GL::EnumType attachment, GL::IntType internalformat,
                      GL::EnumType pixelFormat) const noexcept;

    public:
        inline
        ~GBuffer() noexcept {
            destroy();
        }

        void
        destroy() noexcept;

        void
        draw() noexcept;

        [[nodiscard]] bool
        generate(std::size_t width, std::size_t height) noexcept;

        [[nodiscard]] inline constexpr GL::FramebufferType
        buffer() const noexcept {
            return m_buffer;
        }

        [[nodiscard]] inline constexpr GL::TextureType
        texturePosition() const noexcept {
            return m_position;
        }

        [[nodiscard]] inline constexpr GL::TextureType
        textureNormal() const noexcept {
            return m_normal;
        }

        [[nodiscard]] inline constexpr GL::TextureType
        textureColorSpec() const noexcept {
            return m_colorSpec;
        }
    };

} // namespace gle
