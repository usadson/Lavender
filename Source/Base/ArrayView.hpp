/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <cstdint>

namespace base {

    /**
     * A view on a C-style non-owning array. Used when interacting with or
     * between libraries written in the C language. Note that this class
     * doesn't define rules for object lifetime, constness or pointer safety.
     */
    template<typename Type>
    class ArrayView {
        Type *m_data{};
        std::uint32_t m_size{};

    public:
        [[nodiscard]] ArrayView() noexcept = default;
        [[nodiscard]] ArrayView(ArrayView &&) noexcept = default;
        [[nodiscard]] ArrayView(const ArrayView &) noexcept = default;
        ArrayView &operator=(ArrayView &&) noexcept = default;
        ArrayView &operator=(const ArrayView &) noexcept = default;

        [[nodiscard]] inline constexpr
        ArrayView(Type *data, std::uint32_t size) noexcept
                : m_data(data)
                , m_size(size) {
        }

        [[nodiscard]] inline constexpr std::uint32_t &
        size() noexcept {
            return m_size;
        }

        [[nodiscard]] inline constexpr std::uint32_t
        size() const noexcept {
            return m_size;
        }

        [[nodiscard]] inline constexpr Type *
        data() noexcept {
            return m_data;
        }

        [[nodiscard]] inline constexpr Type *
        data() const noexcept {
            return m_data;
        }

        [[nodiscard]] inline constexpr Type *
        begin() const noexcept {
            return m_data;
        }

        [[nodiscard]] inline constexpr Type *
        end() const noexcept {
            return m_data + m_size;
        }

    };

} // namespace base
