/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <GL/glew.h>

#include "Source/Resources/TextureDescriptor.hpp"

namespace gle {

    class TextureDescriptor
            : public resources::TextureDescriptor {
        GLuint m_textureID;

    public:
        [[nodiscard]] inline constexpr explicit
        TextureDescriptor(GLuint textureID) noexcept
                : m_textureID(textureID)  {
        }

        [[nodiscard]] inline constexpr GLuint
        textureID() const noexcept {
            return m_textureID;
        }
    };

} // namespace gle
