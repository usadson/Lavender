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

#include "Source/Base/ArrayView.hpp"
#include "Source/Base/Error.hpp"
#include "Source/Event/Event.hpp"
#include "Source/Event/EventHandler.hpp"
#include "Source/IO/Format/Image/ImageView.hpp"
#include "Source/Resources/ResourceLocation.hpp"

namespace io::image {

    struct BulkImageLoaderData;

    class BulkImageLoader {
    public:
        using ImageRequestTag = unsigned long;

        friend struct BulkImageLoaderBulkRequestGuard;

        struct Image {
            ~Image();

            [[nodiscard]] constexpr ImageView
            view() const noexcept {
                return m_imageView;
            }

            [[nodiscard]] constexpr std::size_t
            tag() const noexcept {
                return m_tag;
            }

            [[nodiscard]] constexpr base::ArrayView<char>
            data() const noexcept {
                return m_imageView.data();
            }

            [[nodiscard]] constexpr std::size_t
            width() const noexcept {
                return m_imageView.width();
            }

            [[nodiscard]] constexpr std::size_t
            height() const noexcept {
                return m_imageView.height();
            }

        private:
            friend class BulkImageLoader;
            friend struct BulkImageRequestData;

            [[nodiscard]]
            Image(ImageRequestTag, BulkImageLoader *, ImageView) noexcept;

            ImageRequestTag m_tag;
            BulkImageLoader *m_parent;

            ImageView m_imageView;
        };

        struct FinishLoadEvent : event::Event {
            [[nodiscard]]
            FinishLoadEvent(ImageRequestTag tag, base::Error &&error) noexcept
                    : m_tag(tag), m_error(std::move(error)) {
            }

            [[nodiscard]] constexpr const base::Error &
            error() const noexcept {
                return m_error;
            }

            [[nodiscard]] constexpr ImageRequestTag
            tag() const noexcept {
                return m_tag;
            }

        private:
            ImageRequestTag m_tag;
            base::Error m_error;
        };

        [[nodiscard]]
        BulkImageLoader() noexcept;

        ~BulkImageLoader() noexcept;

        event::EventHandler<FinishLoadEvent> onFinishLoad{};

        [[nodiscard]] base::Error
        attachDeferredCallback(ImageRequestTag, std::function<base::Error(ImageRequestTag, Image &)>) noexcept;

        [[nodiscard]] std::size_t
        imagesCurrentlyQueued() const noexcept;

        [[nodiscard]] std::size_t
        imagesFinished() const noexcept;

        [[nodiscard]] base::Error
        invokeDeferredCallbacks() noexcept;

        [[nodiscard]] base::ErrorOr<ImageRequestTag>
        requestImageLoad(std::unique_ptr<resources::ResourceLocation>) noexcept;

        [[nodiscard]] std::optional<base::ErrorOr<Image>>
        getData(ImageRequestTag);

    private:
        std::unique_ptr<BulkImageLoaderData> m_data;
    };

    struct BulkImageLoaderBulkRequestGuard {
        BulkImageLoaderBulkRequestGuard(const BulkImageLoaderBulkRequestGuard &) = delete;
        BulkImageLoaderBulkRequestGuard(BulkImageLoaderBulkRequestGuard &&) = delete;

        BulkImageLoaderBulkRequestGuard(BulkImageLoader &) noexcept;
        ~BulkImageLoaderBulkRequestGuard() noexcept;

    private:
        BulkImageLoader &m_bulkImageLoader;
    };

} // namespace io::image
