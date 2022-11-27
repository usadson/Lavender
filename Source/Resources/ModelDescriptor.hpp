/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 - 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include "Source/Resources/ModelGeometryDescriptor.hpp"
#include "Source/Resources/SkinDescriptor.hpp"

namespace resources {

    class MaterialDescriptor;

    class ModelDescriptor {
        ModelGeometryDescriptor *m_geometryDescriptor;
        MaterialDescriptor *m_materialDescriptor;
        SkinDescriptor *m_skinDescriptor;

    public:
        [[nodiscard]] inline constexpr explicit
        ModelDescriptor(ModelGeometryDescriptor *geometryDescriptor,
                        MaterialDescriptor *materialDescriptor = nullptr,
                        SkinDescriptor *skinDescriptor = nullptr) noexcept
                : m_geometryDescriptor(geometryDescriptor)
                , m_materialDescriptor(materialDescriptor)
                , m_skinDescriptor(skinDescriptor) {
        }

        [[nodiscard]] constexpr MaterialDescriptor *
        materialDescriptor() noexcept {
            return m_materialDescriptor;
        }

        [[nodiscard]] constexpr const MaterialDescriptor *
        materialDescriptor() const noexcept {
            return m_materialDescriptor;
        }

        [[nodiscard]] constexpr ModelGeometryDescriptor *
        geometryDescriptor() noexcept {
            return m_geometryDescriptor;
        }

        [[nodiscard]] constexpr const ModelGeometryDescriptor *
        geometryDescriptor() const noexcept {
            return m_geometryDescriptor;
        }

        [[nodiscard]] constexpr SkinDescriptor *
        skinDescriptor() noexcept {
            return m_skinDescriptor;
        }

        [[nodiscard]] constexpr const SkinDescriptor *
        skinDescriptor() const noexcept {
            return m_skinDescriptor;
        }
    };

} // namespace resources
