/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#pragma once

#include "Shader.hpp"

namespace gle {

    class ShaderProgram {
        unsigned int m_programID{};

        Shader m_vertexShader;
        Shader m_fragmentShader;

    public:
        [[nodiscard]]
        ShaderProgram(Shader &&inVertexShader, Shader &&inFragmentShader) noexcept;

        ~ShaderProgram() noexcept;

        [[nodiscard]] constexpr inline bool
        isValid() const noexcept {
            return m_programID != 0 && m_vertexShader.isValid() && m_fragmentShader.isValid();
        }

        [[nodiscard]] constexpr inline unsigned int
        programID() const noexcept {
            return m_programID;
        }
    };

} // namespace gle
