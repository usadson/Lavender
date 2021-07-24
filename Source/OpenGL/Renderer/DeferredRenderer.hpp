/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 Tristan Gerritsen
 * All Rights Reserved.
 */

#pragma once

#include "Source/Base/Debug.hpp"
#include "Source/ECS/Forward.hpp"
#include "Source/OpenGL/Renderer/Renderer.hpp"
#include "Source/OpenGL/Resources/GBuffer.hpp"
#include "Source/OpenGL/Resources/RenderQuad.hpp"
#include "Source/OpenGL/Shaders/GBufferShader.hpp"
#include "Source/OpenGL/Shaders/LightingPassShader.hpp"

#ifdef LAVENDER_BUILD_DEBUG
#   include "Source/OpenGL/Shaders/Debug/LightingPassDebugShader.hpp"
#endif

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
        setPointLight(std::size_t index, const ecs::PointLight &light) noexcept override;

        [[nodiscard]] bool
        setup() noexcept override;

        void
        syncECS() noexcept;

    private:
        void
        drawGBuffer() noexcept;

        void
        drawLighting() noexcept;

        GBuffer m_gBuffer;

        GBufferShader m_gBufferShader{};
        LightingPassShader m_lightingPassShader{};

        RenderQuad m_renderQuad;

        std::size_t m_ecsUpdateCount{0};

#ifdef LAVENDER_BUILD_DEBUG
        LightingPassDebugShader m_lightingPassDebugShader{};

        [[nodiscard]] bool
        setupDebugEnvironment() noexcept;

        void
        renderMode(RenderMode renderMode) noexcept override;

#else
        [[nodiscard]] inline static constexpr bool
        setupDebugEnvironment() noexcept {
            return true;
        }
#endif
    };

} // namespace gle
