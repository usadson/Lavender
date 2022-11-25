/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <GL/glew.h>

#include "Source/Resources/ModelGeometryDescriptor.hpp"

namespace gle {

    class ModelGeometryDescriptor
            : public resources::ModelGeometryDescriptor {
        /**
         * Note: array buffers (vbo, tbo, etc.) may share memory. Do not
         * blindly call glDeleteBuffers on all of them.
         */

        GLsizei m_vertexCount{};
        GLuint m_vao{};
        GLuint m_vbo{};
        GLuint m_ebo{};
        GLuint m_tbo{};
        GLuint m_nbo{}; // normal buffer
        GLsizei m_indexCount{};

        GLenum m_indexType{};

        GLuint m_tangentBufferObject{};
        GLuint m_bitangentBufferObject{};

    public:
        [[nodiscard]] inline constexpr
        ModelGeometryDescriptor(GLsizei vertexCount, GLuint vao, GLuint vbo, GLuint ebo, GLuint tbo, GLuint nbo, GLsizei indexCount, GLenum indexType) noexcept
                : m_vertexCount(vertexCount)
                , m_vao(vao)
                , m_vbo(vbo)
                , m_ebo(ebo)
                , m_tbo(tbo)
                , m_nbo(nbo)
                , m_indexCount(indexCount)
                , m_indexType(indexType) {
        }

        [[nodiscard]] inline constexpr GLuint
        bitangentBufferObject() const noexcept {
            return m_bitangentBufferObject;
        }

        [[nodiscard]] inline constexpr GLuint
        ebo() const noexcept {
            return m_ebo;
        }

        [[nodiscard]] inline constexpr GLsizei
        indexCount() const noexcept {
            return m_indexCount;
        }

        [[nodiscard]] inline constexpr GLenum
        indexType() const noexcept {
            return m_indexType;
        }

        [[nodiscard]] inline constexpr GLenum
        nbo() const noexcept {
            return m_nbo;
        }

        inline constexpr void
        setTangentAndBitangentBufferObjects(GLuint tangentBufferObject, GLuint bitangentBufferObject) noexcept {
            m_tangentBufferObject = tangentBufferObject;
            m_bitangentBufferObject = bitangentBufferObject;
        }

        [[nodiscard]] inline constexpr GLuint
        tangentBufferObject() const noexcept {
            return m_tangentBufferObject;
        }

        [[nodiscard]] inline constexpr GLuint
        tbo() const noexcept {
            return m_tbo;
        }

        [[nodiscard]] inline constexpr GLuint
        vao() const noexcept {
            return m_vao;
        }

        [[nodiscard]] inline constexpr GLuint
        vbo() const noexcept {
            return m_vbo;
        }

        [[nodiscard]] inline constexpr GLsizei
        vertexCount() const noexcept {
            return m_vertexCount;
        }
    };

} // namespace gle
