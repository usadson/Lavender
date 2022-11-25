/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 - 2022 Tristan Gerritsen <trisan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <string_view>

#include <cstddef> // for std::size_t

#include <nlohmann/json.hpp>

#include "Source/Base/ErrorOr.hpp"

namespace io::format::gltf {

    enum class ComponentType
            : std::size_t {
#define LAVENDER_IO_FORMAT_ITERATE_GLTF_COMPONENT_TYPES(REGISTER_TYPE) \
        REGISTER_TYPE(SIGNED_BYTE, 5120, signed char) \
        REGISTER_TYPE(UNSIGNED_BYTE, 5121, unsigned char) \
        REGISTER_TYPE(SIGNED_SHORT, 5122, signed short) \
        REGISTER_TYPE(UNSIGNED_SHORT, 5123, unsigned short) \
        \
        REGISTER_TYPE(UNSIGNED_INT, 5125, unsigned int) \
        REGISTER_TYPE(FLOAT, 5126, float) \

#define REGISTER_TYPE(enumeration, value, _dataType) enumeration = value,
        LAVENDER_IO_FORMAT_ITERATE_GLTF_COMPONENT_TYPES(REGISTER_TYPE)
#undef REGISTER_TYPE
    };

    [[nodiscard]] constexpr std::size_t
    componentTypeToValue(ComponentType componentType) {
        return static_cast<std::size_t>(componentType);
    }
    
    [[nodiscard]] base::ErrorOr<ComponentType>
    convertComponentType(const nlohmann::json& json) noexcept;

    [[nodiscard]] constexpr std::size_t
    componentTypeGetDataTypeSize(ComponentType componentType) {
        switch (componentType) {
#define CHECK_TYPE(enumeration, _value, dataType) \
        case ComponentType::enumeration: return sizeof(dataType);

            LAVENDER_IO_FORMAT_ITERATE_GLTF_COMPONENT_TYPES(CHECK_TYPE)
#undef CHECK_TYPE
        }

        return 0;
    }

} // namespace io::format::gltf

[[nodiscard]] constexpr std::string_view
toString(io::format::gltf::ComponentType componentType) {
    using namespace io::format::gltf;

    switch (componentType) {
#define CHECK_TYPE(enumeration, value, _dataType) \
        case ComponentType::enumeration: return #enumeration;

        LAVENDER_IO_FORMAT_ITERATE_GLTF_COMPONENT_TYPES(CHECK_TYPE)
#undef CHECK_TYPE
    }

    return "(invalid)";
}
