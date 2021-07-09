/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <cassert>

#include "Source/Resources/ModelGeometryDescriptor.hpp"
#include "Source/Resources/TextureDescriptor.hpp"

namespace resources {

    enum class TextureSlot {
        ALBEDO,
    };

    class ModelDescriptor {
        ModelGeometryDescriptor *m_geometryDescriptor;
        TextureDescriptor *m_albedoDescriptor;

    public:
        [[nodiscard]] inline constexpr explicit
        ModelDescriptor(ModelGeometryDescriptor *geometryDescriptor,
                        TextureDescriptor *albedoDescriptor = nullptr) noexcept
                : m_geometryDescriptor(geometryDescriptor)
                , m_albedoDescriptor(albedoDescriptor) {
        }

        [[nodiscard]] inline constexpr TextureDescriptor *
        albedoTextureDescriptor() noexcept {
            return m_albedoDescriptor;
        }

        [[nodiscard]] inline constexpr const TextureDescriptor *
        albedoTextureDescriptor() const noexcept {
            return m_albedoDescriptor;
        }

        inline constexpr void
        attachTexture(TextureSlot textureSlot, TextureDescriptor *textureDescriptor) noexcept {
            switch (textureSlot) {
            case TextureSlot::ALBEDO:
                m_albedoDescriptor = textureDescriptor;
                break;
#ifndef __clang__
            default:
                assert(false);
#endif
            }
        }

        [[nodiscard]] inline constexpr ModelGeometryDescriptor *
        geometryDescriptor() noexcept {
            return m_geometryDescriptor;
        }

        [[nodiscard]] inline constexpr const ModelGeometryDescriptor *
        geometryDescriptor() const noexcept {
            return m_geometryDescriptor;
        }
    };

} // namespace resources
