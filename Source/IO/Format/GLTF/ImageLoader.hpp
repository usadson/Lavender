/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <map>
#include <vector>

#include <nlohmann/json_fwd.hpp>

#include "Source/Base/Error.hpp"
#include "Source/IO/Format/Image/BulkImageLoader.hpp"

namespace io::format::gltf {

    struct Context;

    struct ImageLoader {
        using RequestTag = image::BulkImageLoader::ImageRequestTag;
        using RequestMap = std::map<std::size_t, RequestTag>;
        using DeferredCallback = std::function<base::Error(image::BulkImageLoader::ImageRequestTag, image::BulkImageLoader::Image &)>;

        [[nodiscard]] explicit
        ImageLoader(Context &) noexcept;

        ~ImageLoader() noexcept;

        [[nodiscard]] base::Error
        preLoadAll() noexcept;

        [[nodiscard]] base::ErrorOr<RequestTag>
        requestImageLoad(std::size_t imageIndex) noexcept;

        [[nodiscard]] base::Error
        subscribeImageLoad(std::size_t imageIndex, DeferredCallback callback) noexcept;

        [[nodiscard]] base::Error
        waitForAll() noexcept;

    private:
        Context &m_context;

        const nlohmann::json &m_gltfImages;

        image::BulkImageLoader m_imageLoader{};
        RequestMap m_requestMap{};
    };

} // namespace io::format::gltf
