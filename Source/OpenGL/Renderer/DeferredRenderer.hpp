/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#pragma once

#include "Source/OpenGL/Renderer/Renderer.hpp"
#include "Source/OpenGL/Resources/GBuffer.hpp"
#include "Source/OpenGL/Resources/RenderQuad.hpp"
#include "Source/OpenGL/Shaders/GBufferShader.hpp"
#include "Source/OpenGL/Shaders/LightingPassShader.hpp"

namespace gle {

    class DeferredRenderer
            : public Renderer {
    public:
        [[nodiscard]] inline constexpr explicit
        DeferredRenderer(Core *core) noexcept
                : Renderer(core) {
        }

        AttributeLocations
        attributeLocations() noexcept override;

        void
        onResize(math::Size2D<std::uint32_t>) noexcept override;

        void
        render() noexcept override;

        [[nodiscard]] bool
        setLight(std::size_t index, math::Vector3f position, math::Vector3f color, float radius) noexcept override;

        [[nodiscard]] bool
        setup() noexcept override;

    private:
        void
        drawGBuffer() noexcept;

        void
        drawLighting() noexcept;

        GBuffer m_gBuffer;

        GBufferShader m_gBufferShader{};
        LightingPassShader m_lightingPassShader{};

        RenderQuad m_renderQuad;
    };

} // namespace gle
