/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <string>

#include "Source/ECS/Forward.hpp"
#include "Source/Math/Transformation.hpp"
#include "Source/Resources/ModelDescriptor.hpp"

namespace ecs {

    class Entity {
        std::string m_name;
        const resources::ModelDescriptor *m_modelDescriptor{};
        math::Transformation m_transformation{};

    public:
        [[nodiscard]] inline explicit
        Entity(std::string &&name, const resources::ModelDescriptor *modelDescriptor,
               math::Transformation transformation={}) noexcept
                : m_name(std::move(name))
                , m_modelDescriptor(modelDescriptor)
                , m_transformation(transformation) {
        }

        [[nodiscard]] inline explicit
        Entity(const std::string &name, const resources::ModelDescriptor *modelDescriptor,
               math::Transformation transformation={}) noexcept
            : m_name(name)
            , m_modelDescriptor(modelDescriptor)
            , m_transformation(transformation) {
        }

        virtual
        ~Entity() noexcept = default;

        virtual void
        onUpdate(float deltaTime) noexcept {
            static_cast<void>(deltaTime);
        }

        /**
         * Not all entities have models, so this can be null!
         */
        [[nodiscard]] inline const resources::ModelDescriptor *
        modelDescriptor() const noexcept {
            return m_modelDescriptor;
        }

        [[nodiscard]] inline const std::string &
        name() const noexcept {
            return m_name;
        }

        [[nodiscard]] inline math::Transformation &
        transformation() noexcept {
            return m_transformation;
        }

        [[nodiscard]] inline const math::Transformation &
        transformation() const noexcept {
            return m_transformation;
        }
    };

} // namespace ecs