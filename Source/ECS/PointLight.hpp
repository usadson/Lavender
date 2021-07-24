/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Source/ECS/Entity.hpp"
#include "Source/Math/Vector.hpp"

namespace ecs {

    struct PointLight
            : public Entity {
        [[nodiscard]] inline
        PointLight(math::Vector3f position, math::Vector3f color, float radius, float intensity,
                   float attenuationConstant, float attenuationLinear, float attenuationExponent) noexcept
                : Entity("PointLight", nullptr, math::Transformation{position})
                , m_color(color)
                , m_radius(radius)
                , m_intensity(intensity)
                , m_attenuationConstant(attenuationConstant)
                , m_attenuationLinear(attenuationLinear)
                , m_attenuationExponent(attenuationExponent) {
        }

        [[nodiscard]] inline constexpr float
        attenuationConstant() const noexcept {
            return m_attenuationConstant;
        }

        [[nodiscard]] inline constexpr float
        attenuationExponent() const noexcept {
            return m_attenuationExponent;
        }

        [[nodiscard]] inline constexpr float
        attenuationLinear() const noexcept {
            return m_attenuationLinear;
        }

        [[nodiscard]] inline constexpr math::Vector3f
        color() const noexcept {
            return m_color;
        }

        [[nodiscard]] inline constexpr float
        intensity() const noexcept {
            return m_intensity;
        }

        [[nodiscard]] inline constexpr float
        radius() const noexcept {
            return m_radius;
        }

    private:
        math::Vector3f m_color;
        float m_radius;
        float m_intensity;
        float m_attenuationConstant;
        float m_attenuationLinear;
        float m_attenuationExponent;
    };

} // namespace ecs