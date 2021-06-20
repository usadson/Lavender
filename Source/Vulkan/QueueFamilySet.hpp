/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <optional>

#include <cstdint>

namespace vke {

    struct QueueFamilySet {
    private:
        std::optional<std::uint32_t> m_graphicsFamily{};
        std::optional<std::uint32_t> m_presentFamily{};

    public:
        [[nodiscard]] inline constexpr bool
        isComplete() const noexcept {
            return m_graphicsFamily.has_value()
                && m_presentFamily.has_value();
        }

        [[nodiscard]] inline constexpr std::uint32_t
        graphicsFamily() const noexcept {
            return m_graphicsFamily.value();
        }
        
        [[nodiscard]] inline constexpr std::uint32_t
        presentFamily() const noexcept {
            return m_presentFamily.value();
        }

        inline constexpr void
        setGraphicsFamily(std::uint32_t graphicsFamily) noexcept {
            m_graphicsFamily = graphicsFamily;
        }

        inline constexpr void
        setPresentFamily(std::uint32_t presentFamily) noexcept {
            m_presentFamily = presentFamily;
        }

    };

} // namespace vke
