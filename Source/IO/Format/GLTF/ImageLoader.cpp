/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#define FMT_HEADER_ONLY

#include "ImageLoader.hpp"

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "Source/GraphicsAPI.hpp"
#include "Source/IO/Format/GLTF/Context.hpp"
#include "Source/IO/Format/GLTF/ResourceInfo.hpp"
#include "Source/Resources/MemoryResourceLocation.hpp"
#include "Source/Resources/ResourceLocateEvent.hpp"

namespace io::format::gltf {
        
    ImageLoader::ImageLoader(Context &context) noexcept
            : m_context(context)
            , m_gltfImages(context.json()["images"]) {
    }

    ImageLoader::~ImageLoader() noexcept = default;

    base::Error
    ImageLoader::preLoadAll() noexcept {
        image::BulkImageLoaderBulkRequestGuard guard{ m_imageLoader };

        for (std::size_t imageIndex = 0; imageIndex < m_gltfImages.size(); ++imageIndex) {
            if (auto error = requestImageLoad(imageIndex).error())
                return error;
        }

        return base::Error::success();
    }

    base::ErrorOr<ImageLoader::RequestTag>
    ImageLoader::requestImageLoad(std::size_t imageIndex) noexcept {
        base::FunctionErrorGenerator errors{ "IOGLTFLibrary", "ImageLoader" };

        if (auto it = m_requestMap.find(imageIndex); it != m_requestMap.end())
            return it->second;

        auto &image = m_gltfImages.at(imageIndex);

        std::unique_ptr<resources::ResourceLocation> resourceLocation;

        if (auto bufferViewIt = image.find("bufferView"); bufferViewIt != image.end()) {
            TRY_GET_VARIABLE(resourceInfo, m_context.resolveResourceInfo(bufferViewIt->get<std::size_t>(), *bufferViewIt))
                resourceLocation = std::make_unique<resources::MemoryNonOwningResourceLocation>(
                    resourceInfo.dataView, false
            );
        }
        else if (auto uriIt = image.find("uri"); uriIt != image.end()) {
            if (!uriIt->is_string()) {
                return errors.error("Load image URI", fmt::format("URI is not a string: {}", uriIt->dump()));
            }

            const auto sv = uriIt->get<std::string_view>();
            TRY_GET_VARIABLE(buffer, m_context.loadURI(sv))
            resourceLocation = std::move(buffer);
        }
        else if (auto imageNameIt = image.find("name"); imageNameIt != image.end()) {
            resources::ResourceLocateEvent resourceLocateEvent{
                resources::ResourceLocateEvent::ResourceType::IMAGE,
                imageNameIt->get<std::string_view>()
            };

            resourceLocateEvent.suggestDirectory(std::filesystem::path(m_context.fileName()).parent_path().string());
            m_context.graphicsAPI().onLocateResource.invoke(resourceLocateEvent).displayErrorMessageBox();
            resourceLocation = std::move(resourceLocateEvent.location());
        }
        else {
            assert(false);
            return errors.error("Parse GLTF image element", "Missing resource fields");
        }

        assert(resourceLocation != nullptr);

        TRY_GET_VARIABLE(requestTag, m_imageLoader.requestImageLoad(std::move(resourceLocation)))
        m_requestMap[imageIndex] = requestTag;
        return requestTag;
    }


    base::Error
    ImageLoader::subscribeImageLoad(std::size_t imageIndex, DeferredCallback callback) noexcept {
        TRY_GET_VARIABLE(request, requestImageLoad(imageIndex))
        return m_imageLoader.attachDeferredCallback(request, std::move(callback));
    }

    base::Error
    ImageLoader::waitForAll() noexcept {
        std::size_t imagesQueued{};
        std::size_t imagesFinished{};
        while (true) {
            TRY(m_imageLoader.invokeDeferredCallbacks())

#ifdef GLTF_DEBUG_WAITING_FOR_TEXTURE_LOADING
            const auto oldQueued = imagesQueued;
            const auto oldFinished = imagesFinished;
#endif // GLTF_DEBUG_WAITING_FOR_TEXTURE_LOADING

            imagesQueued = m_imageLoader.imagesCurrentlyQueued();
            imagesFinished = m_imageLoader.imagesFinished();
            if (imagesQueued == imagesFinished)
                break;

#ifdef GLTF_DEBUG_WAITING_FOR_TEXTURE_LOADING
            if ((imagesQueued - imagesFinished) != (oldQueued - oldFinished))
                fmt::print("[GLTF] Still waiting for {} images...\n", (imagesQueued - imagesFinished));
#endif // GLTF_DEBUG_WAITING_FOR_TEXTURE_LOADING
        }

        return base::Error::success();
    }
    
} // namespace io::format::gltf
