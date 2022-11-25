/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <nlohmann/json.hpp>

#include "Source/Base/ArrayView.hpp"

namespace io::format::gltf {

    struct ResourceInfo {
        // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-accessor
        const nlohmann::json &accessor;

        // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-buffer
        const std::string &buffer;

        //https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-bufferview
        const nlohmann::json& bufferView;

        std::size_t byteOffset;
        std::size_t byteLength;

        base::ArrayView<const char> dataView;
    };

} // namespace io::format::gltf
