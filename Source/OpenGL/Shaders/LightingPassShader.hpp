/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#pragma once

#include "Source/Math/Vector.hpp"
#include "Source/OpenGL/Shaders/ShaderProgram.hpp"
#include "Source/OpenGL/Types.hpp"

namespace gle {

    class LightingPassShader {
    public:
        [[nodiscard]] inline GL::UnsignedIntType
        programID() const noexcept {
            if (m_program == nullptr)
                return 0;
            return m_program->programID();
        }

        [[nodiscard]] bool
        setLight(std::size_t index, math::Vector3f position, math::Vector3f color, float radius) noexcept;

        [[nodiscard]] bool
        setup() noexcept;

    private:
        std::unique_ptr<ShaderProgram> m_program;
    };

} // namespace gle
