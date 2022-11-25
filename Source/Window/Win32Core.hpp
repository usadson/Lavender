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

namespace window {

    struct Win32Data;

    enum class Win32DragDropMethod {
        //
        // Use the pre-Windows 95 method.
        // https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-dragfinish
        //
        DROPFILES,

        //
        // Use the OLE method.
        // https://learn.microsoft.com/en-us/windows/win32/api/ole2/
        // 
        OLE,
    };

    class Win32Core final
        : public WindowAPI {
    public:
        [[nodiscard]]
        Win32Core();

        ~Win32Core() override;

        [[nodiscard]] base::Error
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

        [[nodiscard]] base::Error
        enableDragAndDrop(WindowDragAndDropOption) noexcept override;

        [[nodiscard]] math::Vector2u
        queryFramebufferSize() const noexcept override;

        [[nodiscard]] utils::Version<int>
        queryGLContextVersion() const noexcept override;

        [[nodiscard]] base::Error
        requestClose(CloseRequestedEvent::Reason reason) noexcept override;

        /**
         * Request to the underlying API to enable or disable VSync. Please note
         * that this is just a request, and doesn't have to be honored. The
         * OpenGL driver or OS may reject this request if it wants to.
         */
        void
        requestVSyncMode(bool enabled) noexcept override;

        [[nodiscard]] base::Error 
        setCursor([[maybe_unused]] input::StandardCursor) noexcept override;

        void 
        setDarkMode(DarkModeOption option) noexcept override;

        [[nodiscard]] base::Error 
        setIcon(std::string_view fileName) noexcept override;

        void
        setMouseGrabbed(input::MouseGrabbed) noexcept override;

        [[nodiscard]] base::Error
        setTitle(std::string &&) noexcept override;

        [[nodiscard]] inline constexpr bool
        shouldClose() override {
            return m_shouldClose;
        }

        void preLoop() override;
        void postLoop() override;

        void
        showMessageBox(std::string_view title, std::string_view message) noexcept override;

#ifdef WIN32CORE_ENABLE_WINDOWS_FUNCTIONS
        [[nodiscard]] std::optional<LRESULT>
        handleDropFilesNotification(HDROP) noexcept;

        [[nodiscard]] std::optional<LRESULT>
        handleWindowMessage(platform::win32::WindowNotification, WPARAM, LPARAM);
#endif // WIN32CORE_ENABLE_WINDOWS_FUNCTIONS
        
        void
        displayErrorMessage(std::string_view description, std::source_location) const;

    protected:
        void 
        onVisibilityOptionUpdated() noexcept override;

    private:
        [[nodiscard]] base::Error
        initializeAPI(GraphicsAPI::Name);

        void
        honorUserTheme() noexcept;

        void
        onDeactivate() noexcept;

        void
        putMouseCursorAtCenterOfWindow() noexcept;

        [[nodiscard]] bool 
        shouldUseDarkMode() noexcept;

        [[nodiscard]] static const char *
        translateStandardCursorToCursorName(input::StandardCursor) noexcept;

        std::unique_ptr<Win32Data> m_data;
        bool m_shouldClose{false};
        Win32DragDropMethod m_dragAndDropMethod{};
        std::optional<input::StandardCursor> m_currentStandardCursor{};

#ifdef WIN32CORE_ENABLE_WINDOWS_FUNCTIONS
        void
        handleMouseMove(WPARAM, LPARAM);

        [[nodiscard]] std::optional<LRESULT>
        handleSystemCommand(platform::win32::SystemCommand, LPARAM) noexcept;
#endif // WIN32CORE_ENABLE_WINDOWS_FUNCTIONS

#ifdef LAVENDER_WIN32_HAS_OLE
        [[nodiscard]] base::Error
        oleDragAndDropEnable() const noexcept;

        [[nodiscard]] base::Error
        oleDragAndDropDisable() const noexcept;

        [[nodiscard]] static base::Error
        oleInitialize() noexcept;
#endif // LAVENDER_WIN32_HAS_OLE_DND
    };

} // namespace window
