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

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wconversion"
#   pragma GCC diagnostic ignored "-Wdouble-promotion"
#   pragma GCC diagnostic ignored "-Wduplicated-branches"
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#   pragma GCC diagnostic ignored "-Wsign-conversion"
#   pragma GCC diagnostic ignored "-Wsign-compare"
#   pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.hpp"

#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif

#include "Source/Resources/TextureInput.hpp"

namespace gle {

    [[nodiscard]] inline constexpr GLenum
    translateInputType() noexcept {
        if constexpr (sizeof(unsigned char) == 1)
            return GL_UNSIGNED_BYTE;
        if constexpr (sizeof(unsigned char) == 2)
            return GL_UNSIGNED_SHORT;
        if constexpr (sizeof(unsigned char) == 4)
            return GL_UNSIGNED_INT;
        assert(false);
        return GL_INVALID_ENUM;
    }

    [[nodiscard]] inline constexpr GLenum
    translateTextureFormat(int channelCount) noexcept {
        switch (channelCount) {
            case 1: return GL_RED;
            case 2: return GL_RG;
            case 3: return GL_RGB;
            case 4: return GL_RGBA;
            default:
                assert(false);
                return GL_INVALID_ENUM;
        }
    }

    resources::TextureDescriptor *
    Core::createTexture(const resources::TextureInput &textureInput) noexcept {
        int width, height, channelCount;
        unsigned char *data = stbi_load(std::data(textureInput.fileName), &width, &height, &channelCount, STBI_default);
        if (data == nullptr) {
            std::printf("[GL] TextureLoader: incorrect parameters returned by stbi_load");
            return nullptr;
        }

        if (width < 1 || height < 1 || channelCount < 1) {
            std::printf("[GL] TextureLoader: incorrect parameters returned by stbi_load");
            return nullptr;
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     width, height,
                     0,
                     translateTextureFormat(channelCount),
                     translateInputType(),
                     data);

        stbi_image_free(data);

        if (textureInput.optGenerateMipmap) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        m_textureDescriptors.push_back(std::make_unique<TextureDescriptor>(textureID));
        return m_textureDescriptors.back().get();
    }

} // namespace gle
