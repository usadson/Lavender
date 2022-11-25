/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <filesystem>

#include "Source/Resources/ResourceLocation.hpp"

namespace resources {

    struct FileResourceLocation final
            : public ResourceLocation {
        [[nodiscard]] inline explicit
        FileResourceLocation(std::filesystem::path &&path)
                : ResourceLocation(ResourceLocation::Type::FILE)
                , m_path(std::move(path)) {
        }

        [[nodiscard]] base::ErrorOr<std::string>
        readAllBytes() const noexcept override;

        [[nodiscard]] inline constexpr const std::filesystem::path &
        path() const noexcept {
            return m_path;
        }

    private:
        std::filesystem::path m_path;
    };

} // namespace resources
