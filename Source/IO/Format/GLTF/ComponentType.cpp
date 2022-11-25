/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 - 2022 Tristan Gerritsen <trisan@thewoosh.org>
 * All Rights Reserved.
 */

#include "ComponentType.hpp"

#include <sstream>

#include <nlohmann/json.hpp>

namespace io::format::gltf {

    base::ErrorOr<ComponentType>
    gltf::convertComponentType(const nlohmann::json &json) noexcept {
        base::FunctionErrorGenerator errors{"IOFormatLibrary", "IOGLTFLibrary"};
        if (!json.is_number())
            return errors.error("Convert ComponentType", "JSON-type isn't number");

        if (!json.is_number_unsigned())
            return errors.error("Convert ComponentType", "JSON-type is number, but isn't an unsigned integer");

        auto value = json.get<std::size_t>();
        switch (value) {
#define CHECK_TYPE(enumeration, value, _dataType) case value: return ComponentType::enumeration;
            LAVENDER_IO_FORMAT_ITERATE_GLTF_COMPONENT_TYPES(CHECK_TYPE)
#undef CHECK_TYPE
        default:
            break;
        }

        if (value == 5124)
            return errors.error("Convert ComponentType (integer) to enumeration", "Value is GL_INT (5124), which is disallowed by the GLTF 2.0 specification.");

        std::stringstream stream;
        stream << "Unknown Value: " << value << " (hex: 0x" << std::hex << value << std::dec << ')';
        
        return errors.error("Convert ComponentType (integer) to enumeration", stream.str());
    }

} // namespace io::format::gltf
