/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include "Source/Base/ErrorOr.hpp"

namespace resources {

    struct ResourceLocation {
        enum class Type {
            FILE,
            MEMORY_NON_OWNING,
            MEMORY_OWNING,

            VENDOR_SPECIFIC = 0x1000,
        };

        [[nodiscard]] inline explicit
        ResourceLocation(Type type)
                : m_type(type) {
        }

        virtual
        ~ResourceLocation() noexcept = default;

        [[nodiscard]] inline constexpr Type
        type() const noexcept {
            return m_type;
        }

        [[nodiscard]] virtual base::ErrorOr<std::string>
        readAllBytes() const noexcept = 0;

    private:
        Type m_type;
    };

} // namespace resources
