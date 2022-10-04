/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
* All Rights Reserved.
 */

#include "Win32Glue.hpp"

#include <cstdio>

//#define FMT_SHARED
//#include <fmt/format.h>
#include <array>
#include <source_location>
#include <sstream>
#include <string_view>

#include "OpenGLHelpers.hpp"
#include "Source/OpenGL/Interface/ContextAttributes.hpp"

namespace gle::win32 {

    struct RenderContext {
        [[nodiscard]] inline explicit
        RenderContext(HDC hdc)
            : m_hdc(hdc)
            , m_ctx(wglCreateContext(hdc)) {
        }

        inline void
        makeCurrent() {
            m_oldContext = wglGetCurrentContext();
            wglMakeCurrent(m_hdc, m_ctx);
        }

        inline
        ~RenderContext() {
            if (m_oldContext != nullptr && wglGetCurrentContext() == m_ctx)
                wglMakeCurrent(m_hdc, m_oldContext);

            if (m_ctx)
                wglDeleteContext(m_ctx);
        }

    private:
        HDC m_hdc;
        HGLRC m_ctx;

        HGLRC m_oldContext{};
    };

    [[nodiscard]] HGLRC
    createContextAttribs(HDC hdc, HGLRC hshareContext, const int *attribList) {
        RenderContext renderContext{hdc};
        renderContext.makeCurrent();

        using CreateContextAttribsProc = HGLRC (*)(HDC hDC, HGLRC hshareContext, const int *attribList);
        using RawProc = long long (*)();
        union {
            RawProc raw;
            CreateContextAttribsProc func;
        } data{
            .raw = wglGetProcAddress("wglCreateContextAttribsARB")
        };

        if (data.raw == nullptr) {
            std::puts("[Win32Core] (GLContext) wglCreateContextAttribsARB undefined");
            std::abort();
        }

        return data.func(hdc, hshareContext, attribList);
    }

    std::optional<GLContext>
    GLContext::createContext(HWND windowHandle) {
        std::puts("[Win32Core] (GLContext) createContext() invoked");
        GLContext context{};

        context.m_windowHandleDeviceContext = GetDC(windowHandle);

        int letWindowsChooseThisPixelFormat = ChoosePixelFormat(context.m_windowHandleDeviceContext, &context.m_pixelFormatDescriptor);
        SetPixelFormat(context.m_windowHandleDeviceContext,letWindowsChooseThisPixelFormat,  &context.m_pixelFormatDescriptor);

        //context.m_renderingContext = wglCreateContext(windowHandleDeviceContext);
        std::array<int, 5> attribs{
            static_cast<int>(gle::ContextAttributes::MAJOR_VERSION_ARB),
            4,
            static_cast<int>(gle::ContextAttributes::MINOR_VERSION_ARB),
            5,
            static_cast<int>(gle::ContextAttributes::END_OF_ATTRIBUTE_SEQUENCE),
        };

        context.m_renderingContext = createContextAttribs(
            context.m_windowHandleDeviceContext,
            nullptr,
            attribs.data()
        );

        if (context.m_renderingContext == nullptr) {
            auto error = GetLastError();
            std::printf("[Win32Core] (GLContext) Failed to create context: %lu\n", error);
            return std::nullopt;
        }

        if (!wglMakeCurrent(context.m_windowHandleDeviceContext, context.m_renderingContext)) {
            std::puts("[Win32Core] (GLContext) Failed to make newly created context current.");
            return std::nullopt;
        }

        for (const char *str : std::array{"glGetString", "wglCreateContextAttribsARB"}) {
            auto *ptr = wglGetProcAddress(str);
            auto error = GetLastError();
            std::printf("\"%s\" is at %p error %lu\n", str, reinterpret_cast<const void *>(ptr), error);
        }

        context.m_openGLVersion = gle::openGLVersion();
        return context;
    }

    GLContext::GLContext() noexcept
        : m_pixelFormatDescriptor {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
            PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
            32,                   // Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,                   // Number of bits for the depthbuffer
            8,                    // Number of bits for the stencilbuffer
            0,                    // Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        } {
    }

    GLContext::~GLContext() noexcept {
        if (m_renderingContext != nullptr)
            wglDeleteContext(m_renderingContext);
    }

    void 
    displayErrorMessage(std::string_view description, std::source_location location = {}) {
        LPTSTR lpMessage;

        auto error = GetLastError();
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error, 0,
            reinterpret_cast<LPTSTR>(&lpMessage), 0, nullptr);

        auto errorDescription = [&]() {
            std::stringstream stream;
            stream << "Error: " << std::string_view{lpMessage} << "\nDescription: " << description
                   << "\nIn function: " << location.function_name() << "\nIn file: " << location.file_name() << ':'
                   << location.line();
            return stream.str();
        }();

        MessageBoxA(nullptr, errorDescription.c_str(), TEXT("Win32 Error"), MB_ICONSTOP | MB_OK);

        LocalFree(lpMessage);
    }

    void
    GLContext::swapBuffers() {
        if (!SwapBuffers(m_windowHandleDeviceContext)) {
            displayErrorMessage("SwapBuffers :(");
        }
    }

} // namespace gle::win32
