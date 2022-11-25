/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

#include "Source/Base/ErrorOr.hpp"
#include "Source/Resources/ResourceLocation.hpp"

class GraphicsAPI;

namespace io::format::gltf {

    struct ResourceInfo;

    struct Context {
        [[nodiscard]]
        Context(GraphicsAPI &graphicsAPI, std::string_view fileName, nlohmann::json &&json, std::vector<std::string> &&buffers) noexcept;

        ~Context() noexcept;

        [[nodiscard]] constexpr const std::vector<std::string> &
        buffers() const noexcept {
            return m_buffers;
        }

        [[nodiscard]] constexpr std::string_view
        fileName() const noexcept {
            return m_fileName;
        }

        [[nodiscard]] constexpr GraphicsAPI &
        graphicsAPI() const noexcept {
            return m_graphicsAPI;
        }

        [[nodiscard]] constexpr const nlohmann::json &
        json() const noexcept {
            return m_json;
        }

        [[nodiscard]] base::ErrorOr<std::unique_ptr<resources::ResourceLocation>>
        loadURI(std::string_view) noexcept;

        [[nodiscard]] base::ErrorOr<ResourceInfo>
        resolveResourceInfo(std::size_t bufferViewIndex, const nlohmann::json& accessor) noexcept;

    private:
        GraphicsAPI &m_graphicsAPI;

        std::string_view m_fileName{};
        nlohmann::json m_json;
        std::vector<std::string> m_buffers{};
    };
    
} // namespace io::format::gltf
