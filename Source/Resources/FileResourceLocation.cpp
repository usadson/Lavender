/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#include "FileResourceLocation.hpp"

#define FMT_HEADER_ONLY
#include <fmt/format.h>

#include <Source/IO/FileInput.hpp>

namespace resources {

    base::ErrorOr<std::string>
    FileResourceLocation::readAllBytes() const noexcept {
        base::FunctionErrorGenerator errors{"ResourceLibrary", "FileResourceLocation"};

#define CHECK_FOR_ERROR() \
        if (auto error = fileInput.error(); error != io::Error::NO_ERROR) { \
            const auto description = io::describeError(error); \
            return errors.error("Failed to read file", fmt::format("{}: {}", description.section(), description.code())); \
        } \
        static_cast<void>(0)

        const auto pathStr = path().string();
        io::FileInput fileInput{ pathStr.c_str() };
        CHECK_FOR_ERROR();

        std::string result;
        result.resize(fileInput.size());
        if (!fileInput.read(result.begin(), result.end()))
            CHECK_FOR_ERROR();

        return result;
    }


} // namespace resources
