/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#include "MemoryResourceLocation.hpp"

#include "ThirdParty/base64.hpp"

namespace resources {

    template<typename T = std::vector<char>>
    [[nodiscard]] static base::ErrorOr<T>
    decodeBase64(std::string_view base64) noexcept {
        base::FunctionErrorGenerator errors{ "ResourcesCore", "MemoryResourceLocation" };

        if (base64.length() % 4 != 0)
            return errors.error("Decode BASE64 data", "Invalid data: data length % 4 != 0");

        T data;
        if (!base64::decode(base64, data))
            return errors.error("Decode BASE64 data", "Failed (unknown causes)");

        return data;
    }

    base::ErrorOr<base::ArrayView<const char>>
    MemoryResourceLocation::dataAsNonBase64() noexcept {
        if (!m_isBase64)
            return data();

        if (m_base64Decoded.empty()) {
            const auto encoded = data();
            if (encoded.size() == 0)
                return encoded;

            TRY_GET_VARIABLE(decoded, decodeBase64(std::string_view{ encoded.data(), encoded.size() }))
            m_base64Decoded = decoded;
        }

        return base::ArrayView<const char>{m_base64Decoded.data(), m_base64Decoded.size()};
    }

    base::ErrorOr<std::string>
    MemoryResourceLocation::readAllBytes() const noexcept {
        const auto dataView = data();
        const std::string_view view{ dataView.data(), dataView.size() };
        if (!m_isBase64)
            return std::string(view);

        return decodeBase64<std::string>(view);
    }

    MemoryResourceLocation::~MemoryResourceLocation() noexcept = default;
    MemoryOwningResourceLocation::~MemoryOwningResourceLocation() noexcept = default;
    MemoryNonOwningResourceLocation::~MemoryNonOwningResourceLocation() noexcept = default;

} // namespace resources
