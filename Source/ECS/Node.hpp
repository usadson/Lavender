/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <trisan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <functional>

namespace ecs {

    enum class DidUpdate {
        NO, YES
    };

    class Node {
    public:
        [[nodiscard]] inline constexpr explicit
        Node(Node *parent = nullptr) noexcept
                : m_parent(parent) {
        }
        
        virtual
        ~Node() noexcept = default;

        [[nodiscard]] virtual bool
        isCamera() const noexcept {
            return false;
        }

        [[nodiscard]] virtual bool
        isEntity() const noexcept {
            return false;
        }

        [[nodiscard]] virtual bool
        isLight() const noexcept {
            return false;
        }

        [[nodiscard]] virtual bool
        isScene() const noexcept {
            return false;
        }

        virtual constexpr void
        iterateChildren([[maybe_unused]] const std::function<void(Node &)> &callback) noexcept {
        }

        [[nodiscard]] virtual DidUpdate
        onUpdate([[maybe_unused]] float deltaTime) noexcept {
            return DidUpdate::NO;
        }

        [[nodiscard]] inline constexpr Node *
        parent() const noexcept {
            return m_parent;
        }

        inline constexpr void
        setParent(Node *parent) noexcept {
            m_parent = parent;
        }

    private:
        Node *m_parent;
    };

} // namespace ecs