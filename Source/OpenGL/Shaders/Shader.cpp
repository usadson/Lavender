/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#include "Shader.hpp"

#include <array>
#include <iostream>
#include <stdexcept>
#include <type_traits>

#include <GL/glew.h>

#include "Source/IO/FileInput.hpp"

namespace gle {

    [[nodiscard]] constexpr inline GLenum
    ConvertShaderTypeToGLEnum(ShaderType type) noexcept {
        switch (type) {
        case ShaderType::VERTEX:
            return GL_VERTEX_SHADER;
        case ShaderType::FRAGMENT:
            return GL_FRAGMENT_SHADER;
        default:
#ifdef __clang__
            throw std::runtime_error(std::string("Shader type isn't registered in ") + __PRETTY_FUNCTION__);
#else
            return GL_NONE;
#endif
        }
    }

    Shader::Shader(ConstructionMode constructionMode,
                   ShaderType shaderType,
                   std::string_view sv) {
        static_assert(std::is_same_v<GLuint, decltype(Shader::m_shaderID)>);
        static_assert(std::is_same_v<GLchar, char>);

        if (!createShader(shaderType))
            return;

        if (!loadShaderData(constructionMode, sv)
                || !compileShader()) {
            glDeleteShader(m_shaderID);
            m_shaderID = 0;
        }
    }

    Shader::~Shader() noexcept {
        if (m_shaderID != 0) {
            if (m_programID != 0) {
                glDetachShader(m_programID, m_shaderID);
            }

            glDeleteShader(m_shaderID);
        }
    }

    void
    Shader::attach(std::uint32_t program) noexcept {
        m_programID = program;
        glAttachShader(program, m_shaderID);
    }

    bool
    Shader::createShader(ShaderType shaderType) noexcept {
        m_shaderID = glCreateShader(ConvertShaderTypeToGLEnum(shaderType));
        return m_shaderID != 0;
    }

    void
    Shader::detach() noexcept {
        if (m_programID != 0) {
            glDetachShader(m_programID, m_shaderID);
            m_programID = 0;
        }
    }

    bool
    Shader::compileShader() noexcept {
        glCompileShader(m_shaderID);

        GLint success{};
        glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &success);

        if (success == GL_FALSE) {
            std::array<char, 512> buffer{};
            glGetShaderInfoLog(m_shaderID, static_cast<GLsizei>(std::size(buffer)), nullptr, std::data(buffer));
            std::cout << "[GL] Shader: compile error!\n";
            std::cout << "[GL] Shader: " << std::data(buffer) << '\n';

            return false;
        }

        return true;
    }

    bool
    Shader::loadShaderData(ConstructionMode constructionMode, std::string_view sv) noexcept {
        switch (constructionMode) {
        case ConstructionMode::GLSL_PATH:
            return loadGLSLPath(sv);
        case ConstructionMode::GLSL_SOURCE:
            return loadGLSLSource(sv);
        default:
            std::cout << "[GL] Shader: loadShaderData: unimplemented constructionMode!\n";
            return false;
        }
    }

    bool
    Shader::loadGLSLPath(std::string_view inPath) noexcept {
        const auto path = "Resources/Shaders/" + std::string(inPath);
        io::FileInput input{path};

        if (input.error() != io::Error::NO_ERROR) {
            const auto errorDescription = io::describeError(input.error());
            std::printf("[GL] Shader: failed to loader shader \"%s\", error-code: \"%s\" in \"%s\"\n", path.c_str(),
                        errorDescription.code().data(), errorDescription.section().data());
            return false;
        }

        const auto data = input.read<char>(std::size(input));
        const auto *dataPtr = std::data(data);
        const auto size = static_cast<GLint>(std::size(input));
        glShaderSource(m_shaderID, 1, &dataPtr, &size);
        return glGetError() == GL_NO_ERROR;
    }

    bool
    Shader::loadGLSLSource(std::string_view source) noexcept {
        const char *data = std::data(source);
        glShaderSource(m_shaderID, 1, std::addressof(data), nullptr);
        return true;
    }

} // namespace gle
