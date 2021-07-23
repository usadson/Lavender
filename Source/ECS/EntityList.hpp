/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <memory>
#include <vector>

#include <cassert>
#include <cstddef> // for std::size_t

#include "Source/ECS/Entity.hpp"
#include "Source/ECS/Forward.hpp"

namespace ecs {

    class EntityList {
        friend class Scene;

        std::vector<std::unique_ptr<Entity>> m_entities;

        /**
         * Useful for Graphics APIs that need to rebuild / repack the entity
         * list internally.
         */
        std::size_t m_updateCount{0};

    public:
        inline Entity *
        add(std::unique_ptr<Entity> &&entity) noexcept {
            m_entities.push_back(std::move(entity));
            ++m_updateCount;
            return m_entities.back().get();
        }

        template<typename...Args>
        inline Entity *
        create(Args &&...args) noexcept {
            m_entities.push_back(std::make_unique<Entity>(args...));
            ++m_updateCount;
            return m_entities.back().get();
        }

        [[nodiscard]] inline const std::vector<std::unique_ptr<Entity>> &
        data() const noexcept {
            return m_entities;
        }

        inline void
        remove(Entity *entity) noexcept {
            auto it = std::find_if(std::begin(m_entities), std::end(m_entities),
                                  [entity] (const std::unique_ptr<Entity> &vEntity) {
                return vEntity.get() == entity;
            });

            assert(it != std::end(m_entities));

            m_entities.erase(it);
            ++m_updateCount;
        }

        [[nodiscard]] inline constexpr std::size_t
        updateCount() const noexcept {
            return m_updateCount;
        }
    };

} // namespace ecs