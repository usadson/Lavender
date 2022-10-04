/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
* All Rights Reserved.
*/

#pragma once

#undef UNICODE
#undef _UNICODE

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <optional>

#include <cstdint>

#include "Source/Utils/Version.hpp"

namespace gle::win32 {

    struct GLContext {
        [[nodiscard]] static std::optional<GLContext>
        createContext(HWND windowHandle);

        [[nodiscard]]
        GLContext() noexcept;

        [[nodiscard]] inline constexpr
        GLContext(GLContext &&other) noexcept
                : m_pixelFormatDescriptor(other.m_pixelFormatDescriptor)
                , m_openGLVersion(other.m_openGLVersion)
                , m_renderingContext(other.m_renderingContext)
                , m_windowHandleDeviceContext(other.m_windowHandleDeviceContext) {
            other.m_renderingContext = nullptr;
        }

        inline
        GLContext &operator=(GLContext &&other) noexcept {
            m_pixelFormatDescriptor = other.m_pixelFormatDescriptor;
            m_openGLVersion = other.m_openGLVersion;
            m_renderingContext = other.m_renderingContext;
            m_windowHandleDeviceContext = other.m_windowHandleDeviceContext;

            other.m_renderingContext = nullptr;

            return *this;
        }

        GLContext(const GLContext &) noexcept = delete;
        GLContext &operator=(const GLContext &) noexcept = delete;

        ~GLContext() noexcept;

        [[nodiscard]] inline constexpr utils::Version<int>
        glVersion() const {
            return m_openGLVersion;
        }

        [[nodiscard]] inline constexpr HGLRC
        renderingContext() const {
            return m_renderingContext;
        }

        void
        swapBuffers();

    private:
        PIXELFORMATDESCRIPTOR m_pixelFormatDescriptor;
        utils::Version<int> m_openGLVersion{0, 0, 0};
        HGLRC m_renderingContext{};
        HDC m_windowHandleDeviceContext{};
    };

} // namespace gle::win32
