/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#include "Source/OpenGL/Resources/GBuffer.hpp"

#include <array>
#include <limits>

#include <cstdio>

#include <GL/glew.h>

namespace gle {

    bool
    GBuffer::createDepthBuffer() noexcept {
        glGenRenderbuffers(1, &m_depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
        return glGetError() == GL_NO_ERROR;
    }

    bool
    GBuffer::createTexture(GL::TextureType *destination, GL::EnumType attachment, GL::IntType internalFormat,
                           GL::EnumType pixelFormat) const noexcept {
        GL::TextureType texture;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, GL_RGBA, pixelFormat, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);

        *destination = texture;
        GLenum err = glGetError();
        if (err == GL_NO_ERROR)
            return true;
        std::printf("[GL] GBuffer: createTexture error: %x\n", err);
        return false;
    }

    void
    GBuffer::destroy() noexcept {
        if (m_position != 0)
            glDeleteTextures(1, &m_position);

        if (m_normal != 0)
            glDeleteTextures(1, &m_normal);

        if (m_colorSpec != 0)
            glDeleteTextures(1, &m_colorSpec);

        if (m_depthBuffer != 0)
            glDeleteRenderbuffers(1, &m_depthBuffer);

        if (m_buffer != 0)
            glDeleteFramebuffers(1, &m_buffer);

        m_position = 0;
        m_normal = 0;
        m_colorSpec = 0;

        m_buffer = 0;
    }

    void
    GBuffer::draw() noexcept {
    }

    bool
    GBuffer::generate(std::size_t width, std::size_t height) noexcept {
        if (glGetError() != GL_NO_ERROR) {
            std::puts("[GL] GBuffer: premature OpenGL error!");
            return false;
        }

        destroy();

        while (glGetError() != GL_NO_ERROR)
            ; // clear OpenGL error stack for useless destroy errors

        if (width > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())
                || height > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
            std::printf("[GL] GBuffer: dimensions too large: %lux%lu\n", static_cast<unsigned long>(width),
                    static_cast<unsigned long>(height));
            return false;
        }

        m_width = static_cast<GLsizei>(width);
        m_height = static_cast<GLsizei>(height);

        glGenFramebuffers(1, &m_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);

        if (!createTexture(&m_position, GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_FLOAT)) {
            std::puts("[GL] GBuffer: failed to generate 'position' attachment");
            return false;
        }

        if (!createTexture(&m_normal, GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_FLOAT)) {
            std::puts("[GL] GBuffer: failed to generate 'normal' attachment");
            return false;
        }

        if (!createTexture(&m_colorSpec, GL_COLOR_ATTACHMENT2, GL_RGBA, GL_UNSIGNED_BYTE)) {
            std::puts("[GL] GBuffer: failed to generate 'colorSpec' attachment");
            return false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);
        const std::array<GL::UnsignedIntType, 3> attachments{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, std::data(attachments));

        if (!createDepthBuffer()) {
            std::puts("[GL] GBuffer: failed to generate depth buffer");
            return false;
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::puts("[GL] GBuffer: framebuffer is incomplete");
            return false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return true;
    }

} // namespace gle
