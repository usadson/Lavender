/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen
 * All Rights Reserved.
 */

#pragma once

#include "Source/OpenGL/Renderer/Renderer.hpp"
#include "Source/OpenGL/Resources/GBuffer.hpp"
#include "Source/OpenGL/Resources/RenderQuad.hpp"

namespace gle {

    class SkyBoxRenderer final {
    public:
        [[nodiscard]] explicit
        SkyBoxRenderer(Core *core) noexcept;

        void
        render() noexcept;

    private:
        Core *m_core;
    };

} // namespace gle
