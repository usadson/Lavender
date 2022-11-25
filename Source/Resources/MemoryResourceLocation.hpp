/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <vector>

#include "Source/Base/ArrayView.hpp"
#include "Source/Base/ErrorOr.hpp"
#include "Source/Resources/ResourceLocation.hpp"

namespace resources {

    struct MemoryResourceLocation
            : public ResourceLocation {
        ~MemoryResourceLocation() noexcept override;

        [[nodiscard]] virtual base::ArrayView<const char>
        data() const noexcept = 0;

        [[nodiscard]] virtual base::ErrorOr<std::string>
        readAllBytes() const noexcept;

        [[nodiscard]] virtual base::ErrorOr<base::ArrayView<const char>>
        dataAsNonBase64() noexcept;

    protected:
        [[nodiscard]]
        MemoryResourceLocation(ResourceLocation::Type type, bool isBase64)
                : ResourceLocation(type), m_isBase64(isBase64) {
        }

    private:
        bool m_isBase64;
        std::vector<char> m_base64Decoded{};
    };

    struct MemoryOwningResourceLocation final
            : public MemoryResourceLocation {
        [[nodiscard]]
        MemoryOwningResourceLocation(std::vector<char> &&data, bool isBase64)
                : MemoryResourceLocation(Type::MEMORY_OWNING, isBase64)
                , m_data(std::move(data)) {
        }

        ~MemoryOwningResourceLocation() noexcept override;

        [[nodiscard]] base::ArrayView<const char>
        data() const noexcept override {
            return { m_data.data(), m_data.size() };
        }

    private:
        std::vector<char> m_data;
    };

    struct MemoryNonOwningResourceLocation final
            : public MemoryResourceLocation {
        [[nodiscard]]
        MemoryNonOwningResourceLocation(base::ArrayView<const char> data, bool isBase64)
                : MemoryResourceLocation(Type::MEMORY_NON_OWNING, isBase64)
                , m_data(data) {
        }

        ~MemoryNonOwningResourceLocation() noexcept override;

        [[nodiscard]] base::ArrayView<const char>
        data() const noexcept override {
            return m_data;
        }

    private:
        base::ArrayView<const char> m_data;
    };

} // namespace resources
