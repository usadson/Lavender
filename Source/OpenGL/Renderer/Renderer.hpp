/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 - 2022 Tristan Gerritsen
 * All Rights Reserved.
 */

#pragma once

#include "Source/Base/Error.hpp"
#include "Source/ECS/Forward.hpp"
#include "Source/Math/Size2D.hpp"
#include "Source/Math/Vector.hpp"
#include "Source/OpenGL/Renderer/AttributeLocations.hpp"
#include "Source/OpenGL/Renderer/RenderMode.hpp"

namespace gle {

    class Core;

    struct CapabilitiesRequired {
        bool hasSkin{false};
    };

    class Renderer {
    public:
        enum class ModelType {
            NORMAL,
            WITH
        };
        
        [[nodiscard]] inline constexpr explicit
        Renderer(Core *core) noexcept
                : m_core(core) {
        }

        virtual
        ~Renderer() = default;

        [[nodiscard]] virtual AttributeLocations
        attributeLocations(CapabilitiesRequired = {}) noexcept = 0;

        [[nodiscard]] inline constexpr Core *
        core() noexcept {
            return m_core;
        }

        virtual void
        onResize(math::Size2D<std::uint32_t>) noexcept = 0;

        virtual void
        render() noexcept = 0;

        [[nodiscard]] virtual bool
        setPointLight(std::size_t index, const ecs::PointLight &light) noexcept = 0;

        [[nodiscard]] virtual base::Error
        setup() noexcept = 0;

        [[nodiscard]] inline constexpr RenderMode
        renderMode() const noexcept {
#ifdef LAVENDER_BUILD_DEBUG
            return m_renderMode;
#else
            return RenderMode::DEFAULT;
#endif
        }

#ifdef LAVENDER_BUILD_DEBUG
        inline virtual void
        renderMode(RenderMode renderMode) noexcept {
            m_renderMode = renderMode;
        }
#endif

    private:
        Core *m_core;

#ifdef LAVENDER_BUILD_DEBUG
        RenderMode m_renderMode{};
#endif
    };

} // namespace gle
