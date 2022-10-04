/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <source_location>
#include <string_view>

namespace base {

    struct Error {
        [[nodiscard]] inline static constexpr Error
        success() noexcept {
            return Error{true};
        }

        [[nodiscard]] inline constexpr 
        Error(std::string_view libraryName, std::string_view className, 
              std::string_view attemptedAction, std::string_view description,
              std::source_location location = std::source_location::current()) noexcept
            : m_libraryName(libraryName)
            , m_className(className)
            , m_attemptedAction(attemptedAction)
            , m_description(description)
            , m_sourceLocation(location) {
        }

        [[nodiscard]] inline constexpr std::string_view 
        libraryName() const noexcept {
            return m_libraryName;
        }

        [[nodiscard]] inline constexpr std::string_view
        className() const noexcept {
            return m_className;
        }

        [[nodiscard]] inline constexpr std::string_view 
        attemptedAction() const noexcept {
            return m_attemptedAction;
        }

        [[nodiscard]] inline constexpr std::string_view 
        description() const noexcept {
            return m_description;
        }

        [[nodiscard]] inline constexpr std::source_location
        sourceLocation() const noexcept {
            return m_sourceLocation;
        }

        [[nodiscard]] inline constexpr explicit
        operator bool() const noexcept { 
            return !m_success;
        }

        void
        displayErrorMessageBox() const noexcept;

    private:
        inline constexpr explicit
        Error(bool success)
            : m_success(success) {
        }

        bool m_success{false};
        std::string_view m_libraryName{};
        std::string_view m_className{};
        std::string_view m_attemptedAction{};
        std::string_view m_description{};
        std::source_location m_sourceLocation{};
    };

    class FunctionErrorGenerator {
    public:
        [[nodiscard]] inline constexpr 
        FunctionErrorGenerator(std::string_view libraryName, std::string_view className) noexcept
            : m_libraryName(libraryName)
            , m_className(className) {
        }

        [[nodiscard]] inline constexpr Error 
        error(std::string_view attemptedAction, std::string_view errorDescription,
              std::source_location location = std::source_location::current()) noexcept {
            return Error{m_libraryName, m_className, attemptedAction, errorDescription, location};
        }

        [[nodiscard]] inline Error
        fromErrno(std::string_view attemptedAction, std::source_location location = std::source_location::current()) noexcept {
            return Error{m_libraryName, m_className, attemptedAction, errnoToErrorDescription(), location};
        }

#ifdef LAVENDER_WIN32_SUPPORT_ENABLED
        [[nodiscard]] inline Error
        fromWinError(std::string_view attemptedAction, std::source_location location = std::source_location::current()) noexcept {
            return Error{m_libraryName, m_className, attemptedAction, winErrorToDescription(), location};
        }

        [[nodiscard]] inline Error
        fromWinError(std::string_view attemptedAction, int error, std::source_location location = std::source_location::current()) noexcept {
            return Error{m_libraryName, m_className, attemptedAction, winErrorToDescription(error), location};
        }
#endif // LAVENDER_WIN32_SUPPORT_ENABLED

    private:
        [[nodiscard]] static std::string_view 
        errnoToErrorDescription() noexcept;

        [[nodiscard]] static std::string_view 
        winErrorToDescription() noexcept;

        [[nodiscard]] static std::string_view 
        winErrorToDescription(int error) noexcept;

        std::string_view m_libraryName;
        std::string_view m_className;
    };

} // namespace base
