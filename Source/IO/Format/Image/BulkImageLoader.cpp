/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#ifdef _WIN32
#include <windows.h>  
#endif

#include "BulkImageLoader.hpp"

#include <atomic>
#include <map>
#include <mutex>

#define FMT_HEADER_ONLY
#include <fmt/core.h>

#include <ThirdParty/stb/stb_image.hpp>

#include "Source/Resources/FileResourceLocation.hpp"

#if defined(__cpp_rtti) && __cpp_rtti
#define cast dynamic_cast
#else
#define cast static_cast
#endif

namespace io::image {

    using TagType = BulkImageLoader::ImageRequestTag;

#ifdef _WIN32
    void SetThreadName(DWORD dwThreadID, const char* threadName);
#endif

    using DeferredCallback = std::function<base::Error(TagType, BulkImageLoader::Image &)>;

    struct BulkImageRequestData {
        BulkImageRequestData(BulkImageLoader::ImageRequestTag tag) noexcept
                : m_tag(tag) {
        }

        void
        addDeferredCallback(DeferredCallback &&callback) noexcept {
            m_deferredCallbacks.push_back(callback);
        }

        [[nodiscard]] base::Error
        load(std::unique_ptr<resources::ResourceLocation> resourceLocation) noexcept {
            base::FunctionErrorGenerator errors{ "IOLibrary", "BulkImageRequestData" };

            TRY_GET_VARIABLE(memoryData, resourceLocation->readAllBytes());
            int width{}, height{}, channelCount{4};
            auto *data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(memoryData.data()), static_cast<int>(memoryData.size()), &width, &height, &channelCount, STBI_default);

            if (data == nullptr)
                return errors.error("stbi_load_from_memory", fmt::format("Failed to load from resources::MemoryResourceLocation: {}", stbi_failure_reason()));

            if (!data)
                return errors.error("Verify data", "Illegal Condition: data is still null");

            ColorModel colorModel;
            switch (channelCount) {
                case 1:
                    colorModel = ColorModel::GREYSCALE;
                    break;
                case 2:
                    colorModel = ColorModel::GREYSCALE_PLUS_ALPHA;
                    break;
                case 3:
                    colorModel = ColorModel::RGB;
                    break;
                case 4:
                    colorModel = ColorModel::RGBA;
                    break;
                default:
                    return errors.error("Verify data", fmt::format("Incorrect channelCount: {}, value should be 1 to 4 inclusive.", channelCount));
            }
                

            if (width <= 0) 
                return errors.error("Verify data", fmt::format("Incorrect image width: {}", width));

            if (height <= 0) 
                return errors.error("Verify data", fmt::format("Incorrect image height: {}", height));

            //std::printf("Loaded image @ %p\n", static_cast<const void *>(data));
            
            m_image = BulkImageLoader::Image(m_tag, nullptr, 
                ImageView{
                    base::ArrayView{ reinterpret_cast<char*>(data), static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * static_cast<std::size_t>(channelCount) },
                    colorModel,
                    static_cast<std::size_t>(width),
                    static_cast<std::size_t>(height),
                }
            );
            return base::Error::success();
        }

        [[nodiscard]] constexpr BulkImageLoader::ImageRequestTag
        tag() const noexcept {
            return m_tag;
        }

        [[nodiscard]] std::optional<BulkImageLoader::Image> &
        image() noexcept {
            return m_image;
        }

        [[nodiscard]] const std::optional<BulkImageLoader::Image> &
        image() const noexcept {
            return m_image;
        }

        [[nodiscard]] base::Error
        invokeDeferredCallbacks() noexcept {
            if (!m_image.has_value())
                return base::Error("IOLibrary", "BulkImageRequestData", "invokeDeferredCallback", "image has no value");
            for (auto& callback : m_deferredCallbacks)
                callback(m_tag, m_image.value());
            return base::Error::success();
        }

        [[nodiscard]] constexpr bool
        finished() const noexcept {
            return m_finished;
        }

        constexpr void
        markFinished() noexcept {
            m_finished = true;
        }

    private:
        BulkImageLoader::ImageRequestTag m_tag;
        std::optional<BulkImageLoader::Image> m_image;
        std::vector<DeferredCallback> m_deferredCallbacks{};
        bool m_finished{ false };
    };

    struct BulkImageLoaderData {
        [[nodiscard]] explicit
        BulkImageLoaderData(BulkImageLoader *parent)
                : m_parent(parent) {
        }

        [[nodiscard]] auto &
        map() noexcept {
            return m_map;
        }

        [[nodiscard]] BulkImageRequestData &
        newRequest(TagType tag) noexcept {
            BulkImageRequestData *ref;

            if (m_bulkRequest) {
                ref = (m_map[tag] = std::make_unique<BulkImageRequestData>(tag)).get();
            } else {
                std::lock_guard guard{ m_mutex };
                ref = (m_map[tag] = std::make_unique<BulkImageRequestData>(tag)).get();
            }

            return *ref;
        }

        [[nodiscard]] TagType
        newTag() noexcept {
            if (m_bulkRequest)
                return ++m_currentTagIndex;

            std::lock_guard guard{ m_mutex };
            return ++m_currentTagIndex;
        }

        [[nodiscard]] std::mutex &
        mutex() noexcept {
            return m_mutex;
        }

        std::optional<base::ErrorOr<BulkImageLoader::Image>>
        getData(TagType tag) noexcept {
            base::FunctionErrorGenerator errors{ "IOLibrary", "BulkImageLoaderData" };

            auto run = [&] () -> std::optional<base::ErrorOr<BulkImageLoader::Image>> {
                auto it = m_map.find(tag);
                if (it == m_map.end())
                    return errors.error("Find by tag", "Tag invalid.");

                return std::move(it->second->image());
            };

            if (m_bulkRequest)
                return run();

            std::lock_guard guard{ m_mutex };
            return run();
        }

        constexpr void
        setBulkRequest(bool status) noexcept {
            m_bulkRequest = status;
        }

        [[nodiscard]] constexpr bool
        bulkRequest() noexcept {
            return m_bulkRequest;
        }

        std::atomic_size_t totalQueuedImages{};
        std::atomic_size_t finishedImages{};

    private:
        std::mutex m_mutex{};
        BulkImageLoader *m_parent;

        BulkImageLoader::ImageRequestTag m_currentTagIndex{ 0x6480000 };
        std::map<BulkImageLoader::ImageRequestTag, std::unique_ptr<BulkImageRequestData>> m_map{};

        bool m_bulkRequest{false};
    };

    BulkImageLoader::Image::~Image() {
        if (m_parent && data().data())
            free(data().data());
    }

    BulkImageLoader::Image::Image(ImageRequestTag tag, BulkImageLoader *parent, ImageView imageView) noexcept
            : m_tag(tag), m_parent(parent), m_imageView(imageView) {
    }

    BulkImageLoader::BulkImageLoader() noexcept
            : m_data(std::make_unique<BulkImageLoaderData>(this)) {
    }

    BulkImageLoader::~BulkImageLoader() noexcept = default;

    base::ErrorOr<BulkImageLoader::ImageRequestTag>
    BulkImageLoader::requestImageLoad(std::unique_ptr<resources::ResourceLocation> inLocation) noexcept {
        auto tag = m_data->newTag();
        auto &request = m_data->newRequest(tag);
        ++m_data->totalQueuedImages;

        //fmt::print("Requesting image load #{}\n", tag);
        const auto beginLoad = std::chrono::high_resolution_clock::now();
        
        auto thread = std::thread([beginLoad, m_data = m_data.get(), &request](std::unique_ptr<resources::ResourceLocation> resourceLocation, event::EventHandler<FinishLoadEvent> eventHandler) {
            auto error = request.load(std::move(resourceLocation));

            FinishLoadEvent event{request.tag(), std::move(error)};
            eventHandler.invoke(event).displayErrorMessageBox();
            
            request.markFinished();
            ++m_data->finishedImages;

            static_cast<void>(beginLoad);
            //fmt::print("Requesting image load #{} took {} ms\n", request.tag(), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - beginLoad).count());
        }, std::move(inLocation), event::EventHandler(onFinishLoad));

#ifdef _WIN32
        static std::size_t s_bulkImageLoaderId;
        SetThreadName(GetThreadId(thread.native_handle()), fmt::format("BulkImageLoader #{}", s_bulkImageLoaderId++).c_str());
#endif
        thread.detach();

        return {tag};
    }

    base::Error
    BulkImageLoader::attachDeferredCallback(ImageRequestTag tag, DeferredCallback callback) noexcept {
        base::FunctionErrorGenerator errors{ "IOLibrary", "BulkImageLoader" };

        auto run = [&] {
            const auto it = m_data->map().find(tag);
            if (it == m_data->map().end())
                return errors.error("Find by tag", "Tag invalid.");

            it->second->addDeferredCallback(std::move(callback));
            return base::Error::success();
        };

        if (m_data->bulkRequest()) 
            return run();

        std::lock_guard guard{ m_data->mutex() };
        return run();
    }


    std::optional<base::ErrorOr<BulkImageLoader::Image>>
    BulkImageLoader::getData(ImageRequestTag tag) {
        return m_data->getData(tag);
    }

    std::size_t
    BulkImageLoader::imagesCurrentlyQueued() const noexcept {
        return m_data->totalQueuedImages;
    }

    std::size_t
    BulkImageLoader::imagesFinished() const noexcept {
        return m_data->finishedImages;
        /*assert(!m_data->bulkRequest());

        std::lock_guard guard{ m_data->mutex() };
        std::size_t count = 0;
        for (const auto& entry : m_data->map())
            if (entry.second->finished())
                ++count;
        return count;*/
    }

    [[nodiscard]] base::Error
    BulkImageLoader::invokeDeferredCallbacks() noexcept {
        assert(!m_data->bulkRequest());
        
        std::lock_guard guard{m_data->mutex()};
        for (auto it = m_data->map().begin(); it != m_data->map().end();) {
            if (!it->second->finished()) {
                ++it;
                continue;
            }

            if (it->second->image().has_value()) {
                TRY(it->second->invokeDeferredCallbacks())
            }

            --m_data->totalQueuedImages;
            --m_data->finishedImages;
            it = m_data->map().erase(it);
        }
        
        return base::Error::success();
    }

    BulkImageLoaderBulkRequestGuard::BulkImageLoaderBulkRequestGuard(BulkImageLoader &bulkImageLoader) noexcept
            : m_bulkImageLoader(bulkImageLoader) {
        bulkImageLoader.m_data->mutex().lock();
        bulkImageLoader.m_data->setBulkRequest(true);
    }

    BulkImageLoaderBulkRequestGuard::~BulkImageLoaderBulkRequestGuard() noexcept {
        m_bulkImageLoader.m_data->setBulkRequest(false);
        m_bulkImageLoader.m_data->mutex().unlock();
    }

#ifdef _WIN32
    const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)  
    typedef struct tagTHREADNAME_INFO
    {
        DWORD dwType; // Must be 0x1000.  
        LPCSTR szName; // Pointer to name (in user addr space).  
        DWORD dwThreadID; // Thread ID (-1=caller thread).  
        DWORD dwFlags; // Reserved for future use, must be zero.  
    } THREADNAME_INFO;
#pragma pack(pop)  
    void SetThreadName(DWORD dwThreadID, const char *threadName) {
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = threadName;
        info.dwThreadID = dwThreadID;
        info.dwFlags = 0;
#pragma warning(push)  
#pragma warning(disable: 6320 6322)  
        __try {
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
        }
#pragma warning(pop)  
    }
#endif
    
} // namespace io::image
