/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#define FMT_HEADER_ONLY

#include "Context.hpp"

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "Source/GraphicsAPI.hpp"
#include "Source/IO/Format/GLTF/ResourceInfo.hpp"
#include "Source/Resources/MemoryResourceLocation.hpp"
#include "Source/Resources/ResourceLocateEvent.hpp"

namespace io::format::gltf {

    Context::Context(GraphicsAPI &graphicsAPI, std::string_view fileName, nlohmann::json &&json, std::vector<std::string> &&buffers) noexcept
            : m_graphicsAPI(graphicsAPI)
            , m_fileName(fileName)
            , m_json(std::move(json))
            , m_buffers(std::move(buffers)) {
    }

    Context::~Context() noexcept = default;

    base::ErrorOr<resources::MaterialDescriptor *>
    Context::createMaterialDescriptor() noexcept {
        TRY_GET_VARIABLE(descriptor, m_graphicsAPI.createMaterial());
        m_materials.push_back(descriptor);
        return descriptor;
    }

    base::ErrorOr<std::unique_ptr<resources::ResourceLocation>>
    Context::loadURI(std::string_view uriData) noexcept {
        base::FunctionErrorGenerator errors{"IOGLTFLibrary", "Context"};

        if (uriData.empty())
            return errors.error("Load buffer", "URI data is empty");

        if (!uriData.starts_with("data:")) {
            resources::ResourceLocateEvent locateEvent{
                resources::ResourceLocateEvent::ResourceType::BINARY,
                uriData
            };
            locateEvent.suggestDirectory(std::filesystem::path(m_fileName).parent_path().string());

            TRY(m_graphicsAPI.onLocateResource.invoke(locateEvent))
            if (locateEvent.location() == nullptr)
                return errors.error("Resolve buffer URI", fmt::format("File not found: \"{}\"", uriData));
            return std::move(locateEvent.location());
        }

        std::size_t prefix = 0;

        constexpr const std::string_view dataURIPrefix{ "data:application/octet-stream;base64," };
        constexpr const std::string_view dataImagePNGPrefix{ "data:image/png;base64," };
        if (uriData.starts_with(dataURIPrefix)) {
            prefix = dataURIPrefix.size();
        } else if (uriData.starts_with(dataImagePNGPrefix))
            prefix = dataImagePNGPrefix.size();
        else
            return errors.error("Understand URI", fmt::format("Unknown prefix!\n", uriData));

        const auto base64Data = uriData.substr(prefix);

        /*std::string data;
        if (!base64::decode(base64Data, data)) {
            return errors.error("Decode BASE64", "Failed to decode");
        }*/

        return {
            std::make_unique<resources::MemoryNonOwningResourceLocation>(
                base::ArrayView{base64Data.data(), base64Data.size()}, true
            )
        };
    }

    base::ErrorOr<ResourceInfo>
    Context::resolveResourceInfo(std::size_t bufferViewIndex, const nlohmann::json &accessor) noexcept {
        base::FunctionErrorGenerator errors{ "OpenGLCore", "GLTFLoader" };

        const auto &bufferView = m_json["bufferViews"][bufferViewIndex];

        const auto bufferIndex = bufferView["buffer"].get<std::size_t>();
        const auto &buffer = m_buffers[bufferIndex];

        const auto byteLength = bufferView["byteLength"].get<std::size_t>();
        const auto byteOffset = bufferView["byteOffset"].get<std::size_t>();

        if (bufferIndex >= std::size(m_buffers)) {
#ifndef NDEBUG
            std::printf("GLTFLoader: resolveResourceInfo: bufferIndex(%zu) >= buffersSize(%zu)\n", bufferIndex, std::size(m_buffers));
#endif // NDEBUG
            return errors.error("Resolve ResourceInfo", "bufferIndex >= bufferSize");
        }

//        if (byteOffset < 0 || byteLength <= 0) {
//#ifndef NDEBUG
//#endif // NDEBUG
//            std::printf("GLTFLoader: resolveResourceInfo: byteOffset(%zu) or byteLength(%zu) is invalid\n", byteOffset, byteLength);
//            return errors.error("Resolve ResourceInfo", "bufferOffset < 0 || byteLength <= 0");
//        }

        if (byteOffset + byteLength > std::size(buffer)) {
#ifndef NDEBUG
            std::printf("GLTFLoader: resolveResourceInfo: byteOffset(%zu) + byteLength(%zu) (%zu) >= bufferSize(%zu)\n",
                byteOffset, byteLength, static_cast<std::size_t>(byteOffset + byteLength), std::size(buffer));
#endif // NDEBUG
            return errors.error("Resolve ResourceInfo", "Buffer view out of buffer range");
        }

        return ResourceInfo{
            accessor,
            buffer,
            bufferView,
            byteOffset,
            byteLength,
            base::ArrayView{buffer.data() + byteOffset, static_cast<std::size_t>(byteLength)}
        };
    }


} // namespace io::format::gltf
