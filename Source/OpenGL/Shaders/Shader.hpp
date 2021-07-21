/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#pragma once

#include <string_view>

#include "ShaderType.hpp"

namespace gle {

    class Shader {
        std::uint32_t m_shaderID{};
        std::uint32_t m_programID{};

    public:
        enum class ConstructionMode {
            /**
             * The input sv specifies a path to a file containing GLSL code.
             */
            GLSL_PATH,

            /**
             * The input sv contains GLSL code.
             */
            GLSL_SOURCE
        };

        [[nodiscard]]
        Shader(ConstructionMode, ShaderType, std::string_view);

        Shader(const Shader &) = delete;

        inline
        Shader(Shader &&shader) noexcept
                : m_shaderID(shader.m_shaderID)
                , m_programID(shader.m_programID) {
            shader.m_shaderID = 0;
            shader.m_programID = 0;
        }

        ~Shader() noexcept;

        void
        attach(std::uint32_t program) noexcept;

        void
        detach() noexcept;

        [[nodiscard]] inline constexpr bool
        isValid() const noexcept {
            return m_shaderID != 0;
        }

        [[nodiscard]] inline constexpr std::uint32_t
        shaderID() const noexcept {
            return m_shaderID;
        }

    private:
        [[nodiscard]] bool
        createShader(ShaderType) noexcept;

        [[nodiscard]] bool
        compileShader() noexcept;

        [[nodiscard]] bool
        loadGLSLPath(std::string_view) noexcept;

        [[nodiscard]] bool
        loadGLSLSource(std::string_view) noexcept;

        [[nodiscard]] bool
        loadShaderData(ConstructionMode, std::string_view) noexcept;
    };

} // namespace gle
