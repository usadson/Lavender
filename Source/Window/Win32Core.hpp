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
#include <string_view>

#include "Source/Window/WindowAPI.hpp"

#ifdef WIN32CORE_ENABLE_WINDOWS_FUNCTIONS
#   include "Source/Platform/WinPlatform.hpp"
#   include "Source/Platform/Win32/SystemCommand.hpp"
#endif // WIN32CORE_ENABLE_WINDOWS_FUNCTIONS

#ifndef NDEBUG
#   include <source_location>
#endif // !NDEBUG

namespace window {

    struct Win32Data;

    class Win32Core final
        : public WindowAPI {
    public:
        [[nodiscard]]
        Win32Core();

        ~Win32Core() override;

        [[nodiscard]] bool
        initialize(GraphicsAPI::Name graphicsAPI) override;

#ifdef ENABLE_VULKAN
        [[nodiscard]] bool
        createVulkanSurface(vk::Instance instance,
            const vk::Optional<const vk::AllocationCallbacks> &allocator,
            vk::SurfaceKHR *surfaceDestination) noexcept override;

        [[nodiscard]] base::ArrayView<const char *const>
        getRequiredVulkanInstanceExtensions() noexcept override;
#endif
        [[nodiscard]] inline constexpr bool 
        doesWindowSupportDarkMode() const noexcept override { 
            return true;
        }

        [[nodiscard]] math::Vector2u
        queryFramebufferSize() const noexcept override;

        [[nodiscard]] utils::Version<int>
        queryGLContextVersion() const noexcept override;

        [[nodiscard]] base::Error
        requestClose(window::CloseRequestedEvent::Reason reason) noexcept override;

        /**
         * Request to the underlying API to enable or disable VSync. Please note
         * that this is just a request, and doesn't have to be honored. The
         * OpenGL driver or OS may reject this request if it wants to.
         */
        void
        requestVSyncMode(bool enabled) noexcept override;

        void
        setMouseGrabbed(input::MouseGrabbed) noexcept override;

        [[nodiscard]] inline constexpr bool
        shouldClose() override {
            return m_shouldClose;
        }

        [[nodiscard]] base::Error 
        setIcon(std::string_view fileName) noexcept override;

        void 
        setDarkMode(DarkModeOption option) noexcept;

        void preLoop() override;
        void postLoop() override;

#ifdef WIN32CORE_ENABLE_WINDOWS_FUNCTIONS
        [[nodiscard]] std::optional<LRESULT>
        handleWindowMessage(platform::win32::WindowNotification, WPARAM, LPARAM);
#endif // WIN32CORE_ENABLE_WINDOWS_FUNCTIONS

#ifndef NDEBUG
        void
        displayErrorMessage(std::string_view description, std::source_location) const;
#endif // !NDEBUG

    protected:
        void 
        onVisibilityOptionUpdated() noexcept override;

    private:
        [[nodiscard]] bool
        initializeAPI(GraphicsAPI::Name);

#ifdef WIN32CORE_ENABLE_WINDOWS_FUNCTIONS
        void
        handleMouseMove(WPARAM, LPARAM);

        [[nodiscard]] std::optional<LRESULT>
        handleSystemCommand(platform::win32::SystemCommand, LPARAM) noexcept;
#endif // WIN32CORE_ENABLE_WINDOWS_FUNCTIONS

        void
        honorUserTheme() noexcept;

        [[nodiscard]] bool 
        shouldUseDarkMode() noexcept;

        std::unique_ptr<Win32Data> m_data;
        bool m_shouldClose{false};
    };

} // namespace window
