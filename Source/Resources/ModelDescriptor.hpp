/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <cassert>

#include "Source/Resources/ModelGeometryDescriptor.hpp"
#include "Source/Resources/SkinDescriptor.hpp"
#include "Source/Resources/TextureDescriptor.hpp"

namespace resources {

    enum class TextureSlot {
        ALBEDO,
        NORMAL_MAP,
    };

    class ModelDescriptor {
        ModelGeometryDescriptor *m_geometryDescriptor;
        TextureDescriptor *m_albedoDescriptor;
        TextureDescriptor *m_normalMapDescriptor;
        SkinDescriptor *m_skinDescriptor;

    public:
        [[nodiscard]] inline constexpr explicit
        ModelDescriptor(ModelGeometryDescriptor *geometryDescriptor,
                        TextureDescriptor *albedoDescriptor = nullptr,
                        TextureDescriptor *normalMapDescriptor = nullptr,
                        SkinDescriptor *skinDescriptor = nullptr) noexcept
                : m_geometryDescriptor(geometryDescriptor)
                , m_albedoDescriptor(albedoDescriptor)
                , m_normalMapDescriptor(normalMapDescriptor)
                , m_skinDescriptor(skinDescriptor) {
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
                    return;
                case TextureSlot::NORMAL_MAP:
                    m_normalMapDescriptor = textureDescriptor;
                    return;
            }

            assert(false);
        }

        [[nodiscard]] inline constexpr ModelGeometryDescriptor *
        geometryDescriptor() noexcept {
            return m_geometryDescriptor;
        }

        [[nodiscard]] inline constexpr const ModelGeometryDescriptor *
        geometryDescriptor() const noexcept {
            return m_geometryDescriptor;
        }

        [[nodiscard]] inline constexpr TextureDescriptor *
        normalMapTextureDescriptor() noexcept {
            return m_normalMapDescriptor;
        }

        [[nodiscard]] inline constexpr const TextureDescriptor *
        normalMapTextureDescriptor() const noexcept {
            return m_normalMapDescriptor;
        }

        [[nodiscard]] inline constexpr SkinDescriptor *
        skinDescriptor() noexcept {
            return m_skinDescriptor;
        }

        [[nodiscard]] inline constexpr const SkinDescriptor *
        skinDescriptor() const noexcept {
            return m_skinDescriptor;
        }

        [[nodiscard]] constexpr TextureDescriptor *
        textureAttachment(TextureSlot textureSlot) noexcept {
            switch (textureSlot) {
            case TextureSlot::ALBEDO:
                return m_albedoDescriptor;
            case TextureSlot::NORMAL_MAP:
                return m_normalMapDescriptor;
            }

            assert(false);
            return nullptr;
        }
    };

} // namespace resources
