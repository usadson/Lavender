/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#pragma once

#include <memory>

#include "Source/Math/Vector.hpp"
#include "Source/OpenGL/Shaders/ShaderProgram.hpp"
#include "Source/OpenGL/Shaders/Uniform.hpp"
#include "Source/OpenGL/Types.hpp"

namespace gle {

    class SkyBoxShader {
    public:
        [[nodiscard]] inline GL::UnsignedIntType
        programID() const noexcept {
            if (m_program == nullptr)
                return 0;
            return m_program->programID();
        }

        [[nodiscard]] bool
        setup() noexcept;

    private:
        [[nodiscard]] bool
        resolveAttributeLocation(const char *name, GL::UnsignedIntType *destination) const noexcept;

        std::unique_ptr<ShaderProgram> m_program;

        GL::UnsignedIntType m_attribPosition{};
        GL::UnsignedIntType m_attribTextureCoordinates{};
        GL::UnsignedIntType m_attribNormal{};

        UniformMatrix4 m_uniformTransformation{-1};
        UniformMatrix4 m_uniformProjection{-1};
        UniformMatrix4 m_uniformView{-1};
    };

} // namespace gle
