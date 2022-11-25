/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace input {

    struct DragObject {
        enum class Type {
            FILE_LOCATIONS,
            STRING,
        };

        [[nodiscard]] inline static DragObject
        createWithFileLocations(std::vector<std::string> &&strings) noexcept {
            return DragObject{Type::FILE_LOCATIONS, std::move(strings)};
        }

        [[nodiscard]] inline constexpr Type
        type() const noexcept {
            return m_type;
        }

        [[nodiscard]] inline constexpr const std::vector<std::string> &
        asFileLocations() const noexcept {
            return std::get<std::vector<std::string>>(m_value);
        }

    private:
        [[nodiscard]] inline constexpr
        DragObject(Type type, auto &&data)
                : m_type(type)
                , m_value(std::forward<std::remove_reference_t<decltype(data)>>(data)) {
        }

        Type m_type;
        std::variant<std::string, std::vector<std::string>> m_value;
    };
    
} // namespace input
