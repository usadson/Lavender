/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#pragma once

#include <cstddef> // for std::size_t

#include "Source/OpenGL/Types.hpp"

namespace gle {

    class RenderQuad {
        GL::UnsignedIntType m_vao{};
        GL::UnsignedIntType m_vbo{};

    public:
        ~RenderQuad() noexcept;

        [[nodiscard]] bool
        generate() noexcept;

        void
        draw() const noexcept;
    };

} // namespace gle
