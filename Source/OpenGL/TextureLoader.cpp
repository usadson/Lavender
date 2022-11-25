/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * OpenGL® and the oval logo are trademarks or registered trademarks of
 * Hewlett Packard Enterprise in the United States and/or other
 * countries worldwide.
 */

#include "Source/OpenGL/GLCore.hpp"

#include <cassert>
#include <cstdio> // for std::printf

#include <GL/glew.h>

#include <ThirdParty/base64.hpp>
#include <ThirdParty/stb/stb_image.hpp>

#define FMT_HEADER_ONLY
#include <fmt/core.h>

#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif

#include "Source/Resources/FileResourceLocation.hpp"
#include "Source/Resources/MemoryResourceLocation.hpp"
#include "Source/Resources/TextureInput.hpp"

#if defined(__cpp_rtti) && __cpp_rtti
#define cast dynamic_cast
#else
#define cast static_cast
#endif

namespace gle {

    struct LoadedImage {
        [[nodiscard]] static base::ErrorOr<LoadedImage>
        load(resources::ResourceLocation *resourceLocation) noexcept {
            base::FunctionErrorGenerator errors{"OpenGLCore", "LoadedImage"};

            int width{-1};
            int height{-1};
            int channelCount{3};
            unsigned char *data{nullptr};

            switch (resourceLocation->type()) {
                case resources::ResourceLocation::Type::FILE: {
                    auto* fileLocation = cast<resources::FileResourceLocation*>(resourceLocation);

                    auto pathStr = fileLocation->path().string();
                    if (!std::filesystem::exists(fileLocation->path()))
                        return errors.error("Load from FileResourceLocation", fmt::format("File does not exist: \"{}\"", pathStr));

                    data = stbi_load(pathStr.c_str(), &width, &height, &channelCount, STBI_default);

                    if (data == nullptr)
                        return errors.error("stbi_load", fmt::format("Failed to load image: \"{}\"", pathStr));
                    break;
                }
                case resources::ResourceLocation::Type::MEMORY_OWNING:
                case resources::ResourceLocation::Type::MEMORY_NON_OWNING: {
                    TRY_GET_VARIABLE(memoryData, cast<resources::MemoryResourceLocation*>(resourceLocation)->dataAsNonBase64())
                        assert(memoryData.size() != 0);
                    assert(memoryData.data() != nullptr);
                    data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(memoryData.data()), static_cast<int>(memoryData.size()), &width, &height, &channelCount, STBI_default);

                    if (data == nullptr)
                        return errors.error("stbi_load_from_memory", fmt::format("Failed to load from resources::MemoryResourceLocation: {}", stbi_failure_reason()));
                    break;
                }
                default:
                    if (resourceLocation->type() >= resources::ResourceLocation::Type::VENDOR_SPECIFIC)
                        return errors.error("ResourceLocation", fmt::format("Vendor-specific type not supported: {}", static_cast<std::size_t>(resourceLocation->type())));
                    return errors.error("ResourceLocation", fmt::format("Unknown Type: {}", static_cast<std::size_t>(resourceLocation->type())));
            }

            if (!data)
                return errors.error("Verify data", "Illegal Condition: data is still null");
            if (channelCount < 1 || channelCount > 4)
                return errors.error("Verify data", fmt::format("Incorrect channelCount: {}, value should be 1 to 4 inclusive.", channelCount));
            if (width <= 0)
                return errors.error("Verify data", fmt::format("Incorrect image width: {}", width));
            if (height <= 0)
                return errors.error("Verify data", fmt::format("Incorrect image height: {}", height));

            return LoadedImage{ data, static_cast<std::size_t>(width), static_cast<std::size_t>(height), static_cast<std::size_t>(channelCount) };
        }

        [[nodiscard]] explicit
        LoadedImage(unsigned char *data, std::size_t width, std::size_t height, std::size_t channelCount) noexcept
                : m_data(data)
                , m_width(width)
                , m_height(height)
                , m_channelCount(channelCount) {
            assert(m_data != nullptr);
        }

        LoadedImage(LoadedImage &&image) noexcept
                : m_data(image.m_data)
                , m_width(image.m_width)
                , m_height(image.m_height)
                , m_channelCount(image.m_channelCount) {
            assert(m_data != nullptr);
            image.m_data = nullptr;
        }

        ~LoadedImage() noexcept {
            if (m_data)
                stbi_image_free(m_data);
        }

        [[nodiscard]] constexpr std::size_t
        channelCount() const noexcept {
            return m_channelCount;
        }

        [[nodiscard]] unsigned char *
        data() const noexcept {
            assert(m_data != nullptr);
            return m_data;
        }

        [[nodiscard]] constexpr std::size_t
        width() const noexcept {
            return m_width;
        }

        [[nodiscard]] constexpr std::size_t
        height() const noexcept {
            return m_height;
        }

    private:
        unsigned char *m_data;
        std::size_t m_width;
        std::size_t m_height;
        std::size_t m_channelCount;
    };

    [[nodiscard]] inline constexpr GLenum
    translateInputType() noexcept {
        if constexpr (sizeof(unsigned char) == 1)
            return GL_UNSIGNED_BYTE;
        if constexpr (sizeof(unsigned char) == 2)
            return GL_UNSIGNED_SHORT;
        if constexpr (sizeof(unsigned char) == 4)
            return GL_UNSIGNED_INT;
#ifndef _WIN32
        assert(false);
        return GL_INVALID_ENUM;
#endif
    }

    [[nodiscard]] inline constexpr GLenum
    translateTextureFormat(io::image::ColorModel colorModel) noexcept {
        switch (colorModel) {
        case io::image::ColorModel::GREYSCALE: return GL_RED;
        case io::image::ColorModel::GREYSCALE_PLUS_ALPHA: return GL_RG;
        case io::image::ColorModel::RGB: return GL_RGB;
        case io::image::ColorModel::RGBA: return GL_RGBA;
            default:
                assert(false);
                return GL_INVALID_ENUM;
        }
    }

    base::ErrorOr<resources::SkyboxDescriptor *>
    Core::createSkyBox(const resources::TextureInput &textureInput) noexcept {
#ifdef WORKING_ON_SKYBOX
        base::FunctionErrorGenerator errors{"OpenGLCore", "TextureLoader"};
        int width, height, channelCount;
        unsigned char *data = stbi_load(std::data(textureInput.fileName), &width, &height, &channelCount, STBI_default);
        if (data == nullptr)
            return errors.error("stbi_load", "Failed to load.");

        if (width < 1 || height < 1 || channelCount < 1) 
            return errors.error("stbi_load", "Incorrect parameters returned");

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_3D, textureID);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        constexpr const std::array textureTargets{
            GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
        };

        for (GLenum target : textureTargets) {
            glTexImage2D(target, 0, GL_RGB, width, height, 0, translateTextureFormat(channelCount),
                translateInputType(), data);
        }

        stbi_image_free(data);

        if (textureInput.sampler().generateMipMaps()) {
            glGenerateMipmap(GL_TEXTURE_3D);
        }

        m_skyBoxDescriptors.push_back(std::make_unique<SkyboxDescriptor>(textureID));
        return m_skyBoxDescriptors.back().get();
#else // WORKING_ON_SKYBOX
        static_cast<void>(textureInput);
        return nullptr;
#endif // WORKING_ON_SKYBOX
    }

    [[nodiscard]] static GLenum
    convertTextureCoordinateWrappingMode(resources::TextureCoordinateWrappingMode wrappingMode) {
        switch (wrappingMode) {
            case resources::TextureCoordinateWrappingMode::CLAMP_TO_BORDER:
                return GL_CLAMP_TO_BORDER;
            case resources::TextureCoordinateWrappingMode::CLAMP_TO_EDGE:
                return GL_CLAMP_TO_EDGE;
            case resources::TextureCoordinateWrappingMode::CLAMP_TO_EDGE_MIRRORED:
                return GL_MIRROR_CLAMP_TO_EDGE;
            case resources::TextureCoordinateWrappingMode::REPEAT:
                return GL_REPEAT;
            case resources::TextureCoordinateWrappingMode::REPEAT_MIRRORED:
                return GL_MIRRORED_REPEAT;
        }

        return GL_REPEAT;
    }

    [[nodiscard]] base::Error
    applyTextureSampler(const resources::TextureSampler &sampler)  noexcept {
        if (sampler.generateMipMaps())
            glGenerateMipmap(GL_TEXTURE_2D);

        switch (sampler.magnificationFilter()) {
            case resources::TextureFilter::LINEAR:
            case resources::TextureFilter::CUBIC:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            case resources::TextureFilter::NEAREST:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
        }

        switch (sampler.minifyingFilter()) {
            case resources::TextureFilter::LINEAR:
            case resources::TextureFilter::CUBIC:
                if (sampler.generateMipMaps())
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                else
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                break;
            case resources::TextureFilter::NEAREST:
                if (sampler.generateMipMaps())
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                else
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                break;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, convertTextureCoordinateWrappingMode(sampler.uCoordinateWrappingMode()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, convertTextureCoordinateWrappingMode(sampler.vCoordinateWrappingMode()));

        return base::Error::success();
    }

    base::ErrorOr<resources::TextureDescriptor *>
    Core::createTexture(const resources::TextureInput &textureInput) noexcept {
        base::FunctionErrorGenerator errors{ "OpenGLCore", "TextureLoader" };

        const auto image = textureInput.imageView();

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        //fmt::print("Loading data @ {:p} with size {} width {} height {}\n",
        //    static_cast<const void*>(image.data().data()), _msize(image.data().data()), image.width(), image.height());
        //std::fflush(stdout);

        const auto textureFormat = translateTextureFormat(image.colorModel());
        glTexImage2D(GL_TEXTURE_2D, 0, textureFormat,
                     static_cast<GLsizei>(image.width()), static_cast<GLsizei>(image.height()),
                     0,
                     textureFormat,
                     translateInputType(),
                     image.data().data());
        
        //TRY(applyTextureSampler(textureInput.sampler()))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (GLEW_EXT_texture_filter_anisotropic) {
            float value;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value);
            //value = std::max(4.0f, value);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
        } else
            assert(false);

        m_textureDescriptors.push_back(std::make_unique<TextureDescriptor>(textureID));
        return m_textureDescriptors.back().get();
    }

} // namespace gle
