/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <optional>

#include <cassert>

#include "Source/Base/Error.hpp"

namespace base {

    template <typename T>
    struct ErrorOr {
        [[nodiscard]] constexpr
        ErrorOr(Error &&error)
                : m_error(std::move(error))
                , m_data(std::nullopt) {
            assert(error);
        }

        [[nodiscard]] inline constexpr
        ErrorOr(T &&data)
                : m_error(base::Error::success())
                , m_data(std::move(data)) {
        }

        [[nodiscard]] inline constexpr
        ErrorOr(const T &data)
                : m_error(base::Error::success())
                , m_data(data) {
        }

        [[nodiscard]] inline constexpr
        operator bool() const noexcept {
            return m_data.has_value();
        }

        [[nodiscard]] inline constexpr T *
        operator->() noexcept {
            return m_data.operator->();
        }

        [[nodiscard]] inline constexpr const T *
        operator->() const noexcept {
            return m_data.operator->();
        }

        [[nodiscard]] inline constexpr
        operator base::Error() const noexcept {
            return m_error;
        }

        [[nodiscard]] inline constexpr Error
        error() const noexcept {
            return m_error;
        }

        [[nodiscard]] inline constexpr bool
        failed() const noexcept {
            return !m_data.has_value();
        }

        [[nodiscard]] inline constexpr T &
        get() noexcept {
            return m_data.value();
        }

        [[nodiscard]] inline constexpr const T &
        get() const noexcept {
            return m_data.value();
        }

    private:
        base::Error m_error;
        std::optional<T> m_data;
    };

} // namespace base

#define TRY(expr) \
    if (auto error = (expr)) \
        return error;

#define TRY_GET_VARIABLE(variable, expression) \
    auto tryGetVariable_##variable = (expression); \
    if (tryGetVariable_##variable.failed()) \
        return tryGetVariable_##variable.error(); \
    auto variable = std::move(tryGetVariable_##variable.get());
