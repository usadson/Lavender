/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <utility> // for std::move

#include "Source/ECS/EntityList.hpp"

namespace ecs {

    class Scene {
    public:
        [[nodiscard]] inline explicit
        Scene(ecs::EntityList &&entityList) noexcept
                : m_entityList(std::move(entityList)) {
        }

        [[nodiscard]] inline EntityList &
        entityList() noexcept {
            return m_entityList;
        }

        [[nodiscard]] inline const EntityList &
        entityList() const noexcept {
            return m_entityList;
        }

        inline void
        import(Scene &&scene) noexcept {
            const auto size = std::size(std::data(scene.entityList()));
            const auto start = std::size(m_entityList.m_entities);
            m_entityList.m_entities.reserve(start + size);
            for (std::size_t i = 0; i < size; ++i) {
                m_entityList.m_entities.push_back(std::move(scene.m_entityList.m_entities[i]));
            }
        }

    private:
        EntityList m_entityList;
    };

} // namespace ecs