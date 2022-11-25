/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
* All Rights Reserved.
*
* Vulkan and the Vulkan logo are registered trademarks of the Khronos Group
* Inc.
*/

// #define WIN32_DEBUG_PRINT_UNKNOWN_WINDOW_NOTIFICATIONS

#undef UNICODE
#undef _UNICODE
#define VK_PROTOTYPES
#define VK_USE_PLATFORM_WIN32_KHR
#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#define _WINCON_ // prevent wincon.h inclusion
#define WIN32CORE_ENABLE_WINDOWS_FUNCTIONS

#include <windows.h>
#include <windowsx.h>

#include <shellapi.h>

#include "Source/Platform/Win32/DropTarget.hpp"

#include "Win32Core.hpp"

#include <array>
#include <map>
#include <optional>
#include <source_location>
#include <sstream>

#include <cassert>
#include <cstdio>

//#include <fmt/core.h>
//#include <fmt/xchar.h>
#include <GL/glew.h>
#include <map>

#ifdef ENABLE_VULKAN
#   include "Source/Vulkan/IncludeVulkan.hpp"
#   include <vulkan/vulkan_win32.h>
#endif // ENABLE_VULKAN

#include <ThirdParty/stb/stb_image.hpp>

#include "Source/Base/About.hpp"
#include "Source/Base/Error.hpp"
#include "Source/Input/StandardCursor.hpp"
#include "Source/OpenGL/Win32Glue.hpp"
#include "Source/Platform/WinPlatform.hpp"
#include "Source/Platform/Win32/ConsoleManager.hpp"
#include "Source/Platform/Win32/DirectWindowManager.hpp"
#include "Source/Platform/Win32/Input.hpp"
#include "Source/Platform/Win32/UndocumentedAPIs.hpp"
#include "Source/Platform/Win32/UWP.hpp"

#include "Source/Platform/Win32/DropTarget.hpp"

#include <winsock2.h>

#define DISPLAY_ERROR_MESSAGE(description) displayErrorMessage(description, std::source_location::current())

namespace platform::win32 {

    volatile HWND g_mainWindow{ nullptr };

} // namespace platform::win32

namespace window {

    inline static const char s_windowClassName[]  = "Lavender Window Class";

#ifdef ENABLE_VULKAN
    static const std::array<const char *, 1> s_requiredVulkanExtensions{
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };
#endif // ENABLE_VULKAN

    struct IconData {
        [[nodiscard]] 
        IconData(unsigned char *data, std::uint32_t width, std::uint32_t height)
            : m_data(data)
            , m_width(width)
            , m_height(height) {
        }

        [[nodiscard]] inline constexpr
        IconData(IconData&& other) noexcept
                : m_data(other.m_data)
                , m_width(other.m_width)
                , m_height(other.m_height)
                , m_bitmap(other.m_bitmap)
                , m_handle(other.m_handle) {
            other.m_data = nullptr;
            other.m_bitmap = nullptr;
            other.m_handle = nullptr;
        }

        constexpr IconData &
        operator=(IconData &&other) noexcept {
            m_data = other.m_data;
            m_width = other.m_width;
            m_height = other.m_height;
            m_bitmap = other.m_bitmap;
            m_handle = other.m_handle;

            other.m_data = nullptr;
            other.m_bitmap = nullptr;
            other.m_handle = nullptr;
            
            return *this;
        }

        ~IconData() { 
            if (m_data)
                stbi_image_free(m_data);
        }

        [[nodiscard]] inline constexpr unsigned char *
        data() const noexcept { 
            return m_data;
        }

        [[nodiscard]] inline constexpr std::uint32_t
        width() const noexcept { 
            return m_width;
        }

        [[nodiscard]] inline constexpr std::uint32_t
        height() const noexcept { 
            return m_height;
        }

        [[nodiscard]] base::Error 
        create() {
            m_bitmap = CreateBitmap(static_cast<int>(m_width), static_cast<int>(m_height), 1, 24, m_data);

            base::FunctionErrorGenerator generator{"WindowAPI", "Win32Core"};

            BITMAPV5HEADER bitmapHeader{};
            bitmapHeader.bV5Size = sizeof(bitmapHeader);
            bitmapHeader.bV5Width = LONG(m_width);
            bitmapHeader.bV5Height = -LONG(m_height);
            bitmapHeader.bV5Planes = 1;
            bitmapHeader.bV5BitCount = 32;
            bitmapHeader.bV5Compression = BI_BITFIELDS;
            bitmapHeader.bV5RedMask = 0x00ff0000;
            bitmapHeader.bV5GreenMask = 0x0000ff00;
            bitmapHeader.bV5BlueMask = 0x000000ff;
            bitmapHeader.bV5AlphaMask = 0xff000000;

            unsigned char *target = nullptr;

            auto color = [&] {
                auto dc = GetDC(nullptr);
                auto result = CreateDIBSection(dc, reinterpret_cast<BITMAPINFO *>(&bitmapHeader), DIB_RGB_COLORS,
                    reinterpret_cast<void **>(&target), nullptr, 0);
                ReleaseDC(nullptr, dc);
                return result;
            }();
            

            if (!color)
                return generator.fromWinError("Failed to create RGBA bitmap");

            auto mask = CreateBitmap(static_cast<int>(m_width), static_cast<int>(m_height), 1, 1, nullptr);
            if (!mask) {
                DeleteObject(color);
                return generator.fromWinError("Failed to create mask bitmap");
            }

            auto *source = m_data;
            for (std::size_t i = 0; i < std::size_t(m_width) * std::size_t(m_height); ++i) {
                target[0] = source[2];
                target[1] = source[1];
                target[2] = source[0];
                target[3] = source[3];
                target += 4;
                source += 4;
            }

            ICONINFO iconInfo{};
            iconInfo.fIcon = true;
            iconInfo.xHotspot = 0;
            iconInfo.yHotspot = 0;
            iconInfo.hbmMask = mask;
            iconInfo.hbmColor = color;

            m_handle = CreateIconIndirect(&iconInfo);

            DeleteObject(color);
            DeleteObject(mask);

            if (!m_handle)
                return generator.fromWinError("Failed to create icon handle");

            stbi_image_free(m_data);
            m_data = nullptr;

            return base::Error::success();
        }

        [[nodiscard]] inline constexpr HICON 
        handle() const noexcept { 
            return m_handle;
        }

    private:
        unsigned char *m_data;
        std::uint32_t m_width;
        std::uint32_t m_height;

        HBITMAP m_bitmap{};
        HICON m_handle{};
    };

    struct Win32Data {
        WNDCLASS windowClass{};
        ATOM windowClassAtom{};

        HWND windowHandle{};

        int previousMouseX;
        int previousMouseY;
        float mouseXFactor = 1.0f;
        float mouseYFactor = mouseXFactor;

        int windowWidth{};
        int windowHeight{};

        std::map<input::KeyboardKey, bool> keyboardKeyStates{};

        std::optional<IconData> iconData{};
#ifdef LAVENDER_WIN32_HAS_OLE
        platform::win32::DropTarget dropTarget{};
#endif

        ~Win32Data() {
            if (platform::win32::g_hInstance == nullptr) {
                assert(windowHandle == nullptr);
                assert(windowClassAtom == 0);
            }

            if (windowHandle != nullptr) {
                if (platform::win32::g_mainWindow == windowHandle)
                    platform::win32::g_mainWindow = nullptr;
                CloseWindow(windowHandle);
                DestroyWindow(windowHandle);
            }

            if (windowClassAtom != 0) {
                UnregisterClass(s_windowClassName, platform::win32::g_hInstance);
            }
        }

        std::optional<GraphicsAPI::Name> graphicsAPIUsed{};
        std::optional<gle::win32::GLContext> glContext{};

        struct NotificationName {
            std::string_view enumeration;
            std::string_view winMacro;
        };
        std::map<platform::win32::WindowNotification, NotificationName> notificationNames{
#define REGISTER(name, macro) {platform::win32::WindowNotification::name, NotificationName{#name, #macro}},
            PLATFORM_WIN32_ITERATE_WINDOW_NOTIFICATIONS(REGISTER)
#undef REGISTER
        };
    };

    LRESULT CALLBACK
    WindowProc(HWND windowHandle, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        using namespace platform::win32;
        const auto notification = static_cast<WindowNotification>(uMsg);

        auto *win32Core = reinterpret_cast<Win32Core *>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));

        if (notification == WindowNotification::CREATE && win32Core == nullptr) {
            auto *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
            win32Core = reinterpret_cast<Win32Core *>(pCreate->lpCreateParams);
            SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(win32Core));
            auto dataToVerify = reinterpret_cast<Win32Core *>(GetWindowLongPtrA(windowHandle, GWLP_USERDATA));
            if (dataToVerify != win32Core) {
                auto error = GetLastError();
                std::printf("[Win32Core] Failed to set window user ptr: %lu\n", error);
                win32Core->DISPLAY_ERROR_MESSAGE("Failed to set window user ptr");
            }
        }

        if (win32Core != nullptr) {
            auto result = win32Core->handleWindowMessage(notification, wParam, lParam);
            if (result.has_value())
                return result.value();
        }

        return DefWindowProc(windowHandle, uMsg, wParam, lParam);
    }

    Win32Core::Win32Core(){
        std::puts("[Win32Core] Initialized.");
    }

    Win32Core::~Win32Core() = default;

#ifdef ENABLE_VULKAN
    bool
    Win32Core::createVulkanSurface([[maybe_unused]] vk::Instance instance,
        [[maybe_unused]] const vk::Optional<const vk::AllocationCallbacks> &allocator,
        [[maybe_unused]] vk::SurfaceKHR *surfaceDestination) noexcept {

        VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.pNext = nullptr;
        surfaceInfo.flags = 0;
        surfaceInfo.hinstance = platform::win32::g_hInstance;
        surfaceInfo.hwnd = m_data->windowHandle;

        VkSurfaceKHR surface;
        VkResult result = vkCreateWin32SurfaceKHR(instance, &surfaceInfo,
            allocator ? &static_cast<const vk::AllocationCallbacks *>(allocator)->operator const VkAllocationCallbacks &() : nullptr,
            &surface);

        *surfaceDestination = surface;
        return result == VK_SUCCESS;
    }

    base::ArrayView<const char *const>
    Win32Core::getRequiredVulkanInstanceExtensions() noexcept {
        return {s_requiredVulkanExtensions.data(), std::uint32_t(s_requiredVulkanExtensions.size())};
    }
#endif // ENABLE_VULKAN

    base::Error
    Win32Core::enableDragAndDrop(WindowDragAndDropOption option) noexcept {
        if (option == m_dragAndDropOption)
            return base::Error::success();

        base::FunctionErrorGenerator errors{"WindowAPI", "Win32Core"};
        if (!m_data || !m_data->windowHandle)
            return errors.error("Enable Drag & Drop", "Cannot enable this feature before window initialization.");

#ifdef LAVENDER_WIN32_HAS_OLE
        if (m_dragAndDropMethod == Win32DragDropMethod::OLE) {
            if (option == WindowDragAndDropOption::DISABLED) {
                return oleDragAndDropDisable();
            }

            if (auto error = oleDragAndDropEnable()) {
                error.displayErrorMessageBox();

                // OLE method doesn't work, falling back to WM_DROPFILES
                m_dragAndDropMethod = Win32DragDropMethod::DROPFILES;
            }
        }
#endif

        BOOL enable = option == WindowDragAndDropOption::ENABLED;
        DragAcceptFiles(m_data->windowHandle, enable);

        m_dragAndDropOption = option;
        return base::Error::success();
    }

    base::Error
    Win32Core::initialize(GraphicsAPI::Name graphicsAPI) {
        base::FunctionErrorGenerator errors{"WindowAPI", "Win32Core"};

        static_cast<void>(graphicsAPI);
        if (m_data)
            return base::Error::success();

        auto hInstance = platform::win32::g_hInstance;
        if (hInstance == nullptr) {
            return errors.error("Initialize Win32-based WindowAPI", 
                "Cannot initialize WindowAPI since hInstance is null. This error caused because the Win32Core is invoked, but not via the LavenderWin32 executable, which is using WinMain to instantiate the hInstance value.");
        }

        m_data = std::make_unique<Win32Data>();
        m_data->graphicsAPIUsed = graphicsAPI;
        m_data->windowClass.hInstance = hInstance;
        m_data->windowClass.lpfnWndProc = WindowProc;
        m_data->windowClass.lpszClassName = s_windowClassName;

        if (shouldUseDarkMode())
            m_data->windowClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
        else
            m_data->windowClass.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));

//        std::string windowTitle = fmt::format("{} v{}.{}.{}",
//            base::About::applicationName,
//            base::About::applicationVersion.major(),
//            base::About::applicationVersion.minor(),
//            base::About::applicationVersion.patch()
//        );

        std::string windowTitle{base::About::applicationName};

        m_data->windowClassAtom = RegisterClass(&m_data->windowClass);
        if (m_data->windowClassAtom == 0)
            return errors.fromWinError("Register window class");

        m_data->windowWidth = 1280;
        m_data->windowHeight = 720;

        m_data->windowHandle = CreateWindowEx(
            0,                      // window styles
            s_windowClassName,      // window class name
            windowTitle.c_str(),    // window title
            WS_OVERLAPPEDWINDOW,    // style

            CW_USEDEFAULT,          // position X
            CW_USEDEFAULT,          // position Y
            m_data->windowWidth,    // width
            m_data->windowHeight,   // height

            nullptr,                // parent window
            nullptr,                // menu
            hInstance,              // instance handle
            this
        );

        if (m_data->windowHandle == nullptr)
            return errors.fromWinError("CreateWindowEx");

        platform::win32::g_mainWindow = m_data->windowHandle;

        if (!m_currentStandardCursor.has_value())
            setCursor(input::StandardCursor::ARROW).displayErrorMessageBox();

        honorUserTheme();
        onVisibilityOptionUpdated();
        UpdateWindow(m_data->windowHandle);

        POINT point;
        if (GetCursorPos(&point) && ScreenToClient(m_data->windowHandle, &point)) {
            m_data->previousMouseX = point.x;
            m_data->previousMouseY = point.y;
        }

#ifdef LAVENDER_WIN32_HAS_OLE
        m_dragAndDropMethod = Win32DragDropMethod::OLE;
#else
        m_dragAndDropMethod = Win32DragDropMethod::DROPFILES;
#endif

        if (auto error = initializeAPI(graphicsAPI))
            return error;

        
        m_data->dropTarget.onDragEnter += [&](input::DragEnterEvent &event) {
            return onDragEnter.invoke(event);
        };

        m_data->dropTarget.onDragOver += [&](input::DragOverEvent &event) {
            return onDragOver.invoke(event);
        };

        m_data->dropTarget.onDrop += [&](input::DropEvent &event) {
            return onDrop.invoke(event);
        };

        m_data->dropTarget.onDragLeave += [&](input::DragLeaveEvent &event) {
            return onDragLeave.invoke(event);
        };

        return base::Error::success();
    }

    base::Error
    Win32Core::initializeAPI(GraphicsAPI::Name graphicsAPI) {
        switch (graphicsAPI) {
            case GraphicsAPI::Name::OPENGL:
                m_data->glContext = gle::win32::GLContext::createContext(m_data->windowHandle);
                if (!m_data->glContext.has_value())
                    return base::Error("WindowAPI", "Win32Core", "Create OpenGL context", "Failed to create OpenGL context");
                return base::Error::success();
            case GraphicsAPI::Name::VULKAN:
                // We don't have to do anything in here, since Vulkan has API's
                // to connect the HWND to the VkSurface.
                return base::Error::success();
        }

        return base::Error("WindowAPI", "Win32Core", "Initialize GraphicsAPI", "Unknown API");
    }

#ifdef LAVENDER_WIN32_HAS_OLE
    base::Error
    Win32Core::oleDragAndDropEnable() const noexcept {
        if (auto error = oleInitialize())
            return error;

        base::FunctionErrorGenerator errors{"WindowsAPI", "Win32Core"};

        switch (RegisterDragDrop(m_data->windowHandle, &m_data->dropTarget)) {
            case S_OK:
                return base::Error::success();
            case DRAGDROP_E_INVALIDHWND:
                return errors.error(
                    "RegisterDragDrop", "DRAGDROP_E_INVALIDHWND: Invalid handle returned in the hwnd parameter.");
            case DRAGDROP_E_ALREADYREGISTERED:
                return errors.error("RegisterDragDrop",
                    "DRAGDROP_E_ALREADYREGISTERED: The specified window has already been registered as a drop target.");
            case E_OUTOFMEMORY:
                return errors.error("RegisterDragDrop", "E_OUTOFMEMORY: Insufficient memory for the operation.");
            default:
                return errors.error("RegisterDragDrop", "Unknown Error!");
        }
    }

    base::Error
    Win32Core::oleDragAndDropDisable() const noexcept {
        base::FunctionErrorGenerator errors{"WindowsAPI", "Win32Core"};

        switch (RevokeDragDrop(m_data->windowHandle)) {
            case S_OK:
                return base::Error::success();
            case DRAGDROP_E_INVALIDHWND:
                return errors.error(
                    "RevokeDragDrop", "DRAGDROP_E_INVALIDHWND: Invalid handle returned in the hwnd parameter.");
            case DRAGDROP_E_NOTREGISTERED:
                return errors.error("RevokeDragDrop", "DRAGDROP_E_NOTREGISTERED: An attempt was made to revoke a drop target that has not been registered.");
            case E_OUTOFMEMORY:
                return errors.error("RevokeDragDrop", "E_OUTOFMEMORY: There is insufficient memory for the operation.");
            default:
                return errors.error("RevokeDragDrop", "Unknown Error!");
        }
    }

    base::Error
    Win32Core::oleInitialize() noexcept {
        auto result = OleInitialize(nullptr);

        base::FunctionErrorGenerator errors{"WindowAPI", "Win32Core"};
        switch (result) {
            case S_OK:
            case S_FALSE:
                return base::Error::success();
            case OLE_E_WRONGCOMPOBJ:
                return errors.error("OleInitialize",
                    "OLE_E_WRONGCOMPOBJ: The versions of COMPOBJ.DLL and OLE2.DLL on your machine are incompatible with each other.");
            case RPC_E_CHANGED_MODE:
                return errors.error("OleInitialize",
                    "RPC_E_CHANGED_MODE: A previous call to CoInitializeEx specified the concurrency model for this "
                    "apartment as multithread apartment (MTA). This could also mean that a change from neutral "
                    "threaded apartment to single threaded apartment occurred.");
            default:
                return errors.error("OleInitialize", "Unknown error");
        }
    }

#endif // LAVENDER_WIN32_HAS_OLE_DND

    void
    Win32Core::preLoop() {
        MSG msg{};

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void
    Win32Core::postLoop() {
        if (m_data->glContext)
            m_data->glContext->swapBuffers();
    }

    math::Vector2u
    Win32Core::queryFramebufferSize() const noexcept {
        RECT rect;
        GetClientRect(m_data->windowHandle, &rect);
        return {
            static_cast<std::uint32_t>(rect.right - rect.left),
            static_cast<std::uint32_t>(rect.bottom - rect.top)
        };
    }

    utils::Version<int>
    Win32Core::queryGLContextVersion() const noexcept {
        if (m_data->glContext.has_value())
            return m_data->glContext->glVersion();
        return {0, 0, 0};
    }

    base::Error
    Win32Core::requestClose(window::CloseRequestedEvent::Reason reason) noexcept {
        CloseRequestedEvent event{this, reason};
        
        if (auto error = onCloseRequested.invoke(event))
            return error;

        if (event.cancelStatus() == event::CancelStatus::NOT_CANCELLED)
            m_shouldClose = true;
        
        return base::Error::success();
    }

    void
    Win32Core::requestVSyncMode(bool enabled) noexcept {
        static_cast<void>(enabled);
    }

    base::Error
    Win32Core::setCursor(input::StandardCursor standardCursor) noexcept {
        base::FunctionErrorGenerator errors{"WindowAPI", "Win32Core"};

        const char *cursorName = translateStandardCursorToCursorName(standardCursor);
        if (cursorName == nullptr)
            return errors.error("Translate input::StandardCursor to cursorName for LoadCursorA", "Unexpected value");

        auto cursor = LoadCursorA(nullptr, cursorName);
        if (cursor == nullptr)
            return errors.fromWinError("LoadCursorA");

        SetCursor(cursor);
        m_currentStandardCursor = standardCursor;

        if (m_mouseGrabbed == input::MouseGrabbed::YES)
            ShowCursor(FALSE);

        return base::Error::success();
    }

    void
    Win32Core::setMouseGrabbed(input::MouseGrabbed mouseGrabbed) noexcept {
        const bool wasGrabbed = m_mouseGrabbed == input::MouseGrabbed::YES;
        m_mouseGrabbed = mouseGrabbed;

        switch (mouseGrabbed) {
            case input::MouseGrabbed::NO:
                if (!ReleaseCapture() && !wasGrabbed) {
#ifndef NDEBUG
                    DISPLAY_ERROR_MESSAGE("Failed to release mouse capture");
#endif // !NDEBUG
                }
                ShowCursor(true);
                break;
            case input::MouseGrabbed::YES:
                SetCapture(m_data->windowHandle);
                if (GetCapture() != m_data->windowHandle)
                    DISPLAY_ERROR_MESSAGE("Failed to set mouse grabbed");
                else {
                    ShowCursor(false);
                    putMouseCursorAtCenterOfWindow();
                }
                break;
        }
    }

    base::Error
    Win32Core::setTitle(std::string &&title) noexcept {
        base::FunctionErrorGenerator errors{"WindowAPI", "Win32Core"};
        if (!m_data || !m_data->windowHandle)
            return errors.error("Access window data", "Not initialized.");

        if (!SetWindowTextA(m_data->windowHandle, title.c_str()))
            return errors.fromWinError("Set window title");

        return base::Error::success();
    }

    void Win32Core::showMessageBox(std::string_view inTitle, std::string_view inMessage) noexcept {
        std::thread([this, title = std::string(inTitle), message = std::string(inMessage)] {
            MessageBoxA(
                nullptr, 
                message.c_str(), 
                title.c_str(), 
                MB_ICONINFORMATION | MB_OK
            );
        }).detach();
    }


    //
    // Private Functions
    //
    void
    Win32Core::displayErrorMessage(std::string_view description, std::source_location location) const {
        LPTSTR lpMessage;

        auto error = GetLastError();
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                       nullptr, error, 0, reinterpret_cast<LPTSTR>(&lpMessage), 0, nullptr);

        auto errorDescription = [&] () {
            std::stringstream stream;
            stream << "Error: " << std::string_view{lpMessage} << "\nDescription: " << description
                   << "\nIn function: " << location.function_name() << "\nIn file: " << location.file_name() << ':'
                   << location.line();
            return stream.str();
        }();

        MessageBoxA(m_data ? m_data->windowHandle : nullptr,
            errorDescription.c_str(),
            TEXT("Win32 Error"),
            MB_ICONSTOP | MB_OK);

        LocalFree(lpMessage);
    }

    std::optional<LRESULT>
    Win32Core::handleDropFilesNotification(HDROP drop) noexcept {
        if (m_dragAndDropOption != WindowDragAndDropOption::ENABLED
                    || m_dragAndDropMethod != Win32DragDropMethod::DROPFILES)
            return std::nullopt;

        base::FunctionErrorGenerator errors{"WindowAPI", "Win32Core"};

        std::array<char, 128> buffer{};

        if (DragQueryFileA(drop, 0, buffer.data(), static_cast<UINT>(buffer.size())) == 0) {
            errors.fromWinError("DragQueryFileA").displayErrorMessageBox();
            return std::nullopt;
        }

        std::string_view fileName{buffer.data()};
        if (!fileName.ends_with(".gltf")) {
            
        }
        
        return 0;
    }


    std::optional<LRESULT>
    Win32Core::handleWindowMessage(platform::win32::WindowNotification notification,
                                   [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam) {
        using namespace platform::win32;

        switch (notification) {
            case WindowNotification::CLOSE: {
                if (auto error = requestClose(window::CloseRequestedEvent::Reason::UNKNOWN))
                    error.displayErrorMessageBox();
                break;
            }
            case WindowNotification::KEYBOARD_KEY_PRESSED: {
                if (auto key = platform::win32::Input::translateKey(wParam)) {
                    input::KeyPressedEvent event{key.value()};
                    if (auto error = onKeyPressed.invoke(event))
                        error.displayErrorMessageBox();
                }
                break;
            }
            case WindowNotification::KEYBOARD_KEY_RELEASED: {
                if (auto key = platform::win32::Input::translateKey(wParam)) {
                    input::KeyReleasedEvent event{key.value()};
                    if (auto error = onKeyReleased.invoke(event))
                        error.displayErrorMessageBox();
                }
                break;
            }
            case WindowNotification::MOUSE_LEFT_BUTTON_PRESSED:
                mouseCallback()(input::MouseUpdate{input::MouseButton::LEFT, true});
                break;
            case WindowNotification::MOUSE_LEFT_BUTTON_RELEASED:
                mouseCallback()(input::MouseUpdate{input::MouseButton::LEFT, false});
                break;
            case WindowNotification::MOUSE_RIGHT_BUTTON_PRESSED:
                mouseCallback()(input::MouseUpdate{input::MouseButton::RIGHT, true});
                break;
            case WindowNotification::MOUSE_RIGHT_BUTTON_RELEASED:
                mouseCallback()(input::MouseUpdate{input::MouseButton::RIGHT, false});
                break;
            case WindowNotification::MOUSE_MIDDLE_BUTTON_PRESSED:
                mouseCallback()(input::MouseUpdate{input::MouseButton::MIDDLE, true});
                break;
            case WindowNotification::MOUSE_MIDDLE_BUTTON_RELEASED:
                mouseCallback()(input::MouseUpdate{input::MouseButton::MIDDLE, false});
                break;
            case WindowNotification::MOUSE_MOVED:
                handleMouseMove(wParam, lParam);
                break;

            case WindowNotification::DWM_COLORIZATION_COLOR_CHANGED:
                std::printf("Color Changed: %llx\n", wParam);
                honorUserTheme();
                break;
            case WindowNotification::SETTING_CHANGE: {
                std::string_view key{reinterpret_cast<const char *>(lParam)};

                if (key == "ImmersiveColorSet") {
                    std::printf("wParam= %llx\n", wParam);
                    honorUserTheme();
                } else {
                    std::printf("Win.ini changed: %s\n", key.data());
                }
                break;
            }
            case WindowNotification::SET_FOCUS: {
                FocusGainedEvent event{this};
                if (auto error = onFocusGained.invoke(event))
                    error.displayErrorMessageBox();
                break;
            }
            case WindowNotification::KILL_FOCUS: {
                setMouseGrabbed(input::MouseGrabbed::NO);

                FocusLostEvent event{this};
                if (auto error = onFocusLost.invoke(event))
                    error.displayErrorMessageBox();
                break;
            }
            case WindowNotification::LOST_MOUSE_CAPTURE:
                setMouseGrabbed(input::MouseGrabbed::NO);
                break;
            case WindowNotification::MOUSE_HORIZONTAL_WHEEL: {
                /*onHorizontalScroll.invoke(input::ScrollEvent{
                    .delta = static_cast<double>() / SCROLL
                    })*/
                break;
            }
            case WindowNotification::SHOW_WINDOW: {
                auto *hWnd = m_data->windowHandle;
                if (!GetLayeredWindowAttributes(hWnd, nullptr, nullptr, nullptr)) {
                    SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
                    DefWindowProc(hWnd, WM_ERASEBKGND, reinterpret_cast<WPARAM>(GetDC(hWnd)), lParam);
                    SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
                    AnimateWindow(hWnd, 200, AW_ACTIVATE | AW_BLEND);
                    return 0;
                }
                break;
            }
            case WindowNotification::SYSTEM_COMMAND:
                if (auto result = handleSystemCommand(static_cast<SystemCommand>(wParam), lParam))
                    return result.value();
                break;
            case WindowNotification::KEYBOARD_SYSTEM_KEY_PRESSED: {
                if (auto key = Input::translateKey(wParam)) { 
                    if (key.value() == input::KeyboardKey::F4) {
                        window::CloseRequestedEvent event{this, window::CloseRequestedEvent::Reason::SHORTCUT_ALT_F4_PRESSED};
                        if (auto error = onCloseRequested.invoke(event))
                            error.displayErrorMessageBox();
                    }
                }
                break;
            }
            case WindowNotification::SIZE: {
                ResizeEvent::SizeType fromSize{
                    static_cast<std::uint32_t>(m_data->windowWidth), 
                    static_cast<std::uint32_t>(m_data->windowHeight)
                };

                ResizeEvent::SizeType toSize{
                    LOWORD(lParam), 
                    HIWORD(lParam)
                };

                if (toSize.width() == 0 || toSize.height() == 0)
                    break;

                m_data->windowWidth = toSize.width();
                m_data->windowHeight = toSize.height();

                if (!m_shouldClose) {
                    window::ResizeEvent event{this, fromSize, toSize};
                    if (auto error = onResize.invoke(event))
                        error.displayErrorMessageBox();
                }

                break;
            }
            case WindowNotification::DROP_FILES:
                if (auto result = handleDropFilesNotification(reinterpret_cast<HDROP>(wParam)))
                    return result.value();
                break;
            case WindowNotification::CANCEL_MODE:
                m_mouseGrabbed = input::MouseGrabbed::NO;
                break;
            case WindowNotification::ACTIVATE: {
                if (wParam == WA_INACTIVE) {
                    onDeactivate();
                }
                break;
            }
            case WindowNotification::UDP_SOCKET_MESSAGE: {
                auto event = WSAGETSELECTEVENT(lParam);
                auto error = WSAGETSELECTERROR(wParam);
                std::printf("[UDP_SOCKET_MESSAGE] event=%hu error=%hu\n", event, error);
                break;
            }
            case WindowNotification::NON_CLIENT_AREA_MOUSE_MOVE:
            case WindowNotification::NON_CLIENT_AREA_HIT_TEST: 
            case WindowNotification::SET_CURSOR:
            case WindowNotification::KEYBOARD_SYSTEM_KEY_RELEASED:
            case WindowNotification::KEYBOARD_CHARACTER_TRANSLATED:
            case WindowNotification::WINDOW_POSITION_CHANGED:
            case WindowNotification::WINDOW_POSITION_CHANGING:
            case WindowNotification::NON_CLIENT_AREA_CALCULATE_SIZE:
            case WindowNotification::NON_CLIENT_AREA_PAINT:
            case WindowNotification::NON_CLIENT_AREA_MOUSE_LEFT_BUTTON_PRESSED:
            case WindowNotification::NON_CLIENT_AREA_MOUSE_LEFT_BUTTON_RELEASED:
            case WindowNotification::NON_CLIENT_AREA_ACTIVATE:
            case WindowNotification::SIZING:
            case WindowNotification::MOVE:
            case WindowNotification::MOVING:
            case WindowNotification::PAINT:
            case WindowNotification::ACTIVATE_APP:
            case WindowNotification::GET_ICON:
            case WindowNotification::INPUT_METHOD_EDITOR_SET_CONTEXT:
                // ignore output spam
                break;
            default:
#ifdef WIN32_DEBUG_PRINT_UNKNOWN_WINDOW_NOTIFICATIONS
                std::printf("notif %x or ", static_cast<int>(notification));
                if (auto it = m_data->notificationNames.find(notification); it != m_data->notificationNames.end())
                    std::puts(&*it->second.enumeration.begin());
                else {
                    std::printf("wParam=%llu lParam=%llu\n", std::size_t(wParam), std::size_t(lParam));
                }
#endif // WIN32_DEBUG_PRINT_UNKNOWN_WINDOW_NOTIFICATIONS
                break;
        }

        return std::nullopt;
    }

    void
    Win32Core::handleMouseMove([[maybe_unused]] WPARAM wParam, LPARAM lParam) {
        auto x = GET_X_LPARAM(lParam);
        auto y = GET_Y_LPARAM(lParam);

        float moveX, moveY;
        const auto prevX = m_data->previousMouseX;
        const auto prevY = m_data->previousMouseY;

        if (prevX == x && prevY == y)
            return;

        if (m_mouseGrabbed == input::MouseGrabbed::YES) {
            int deltaX = x - m_data->windowWidth / 2;
            int deltaY = y - m_data->windowHeight / 2;

            moveX = static_cast<float>(-deltaX) * m_data->mouseXFactor;
            moveY = static_cast<float>(-deltaY) * m_data->mouseYFactor,

            putMouseCursorAtCenterOfWindow();
        } else {
            moveX = static_cast<float>(x - prevX) * m_data->mouseXFactor;
            moveY = static_cast<float>(y - prevY) * m_data->mouseYFactor;

            m_data->previousMouseX = x;
            m_data->previousMouseY = y;
        }

        if (!mouseCallback())
            return;

        if (moveX != 0 || moveY != 0) {
            mouseCallback()(input::MouseUpdate{
                input::MouseButton::NONE,
                false,
                moveX,
                moveY,
            });
        }
    }

    std::optional<LRESULT> 
    Win32Core::handleSystemCommand(platform::win32::SystemCommand systemCommand, LPARAM lParam) noexcept {
        std::printf("[SystemCommand] %s => %llu\n", toString(systemCommand).data(), lParam);

        if (systemCommand == platform::win32::SystemCommand::CLOSE) {
            auto reason = window::CloseRequestedEvent::Reason::UNKNOWN;
            if (lParam != 0)
                reason = window::CloseRequestedEvent::Reason::USER_PRESSED_CLOSE_BUTTON;

            window::CloseRequestedEvent event{this, reason};
            if (auto error = onCloseRequested.invoke(event))
                error.displayErrorMessageBox();
        }

        return std::nullopt;
    }

    void
    Win32Core::honorUserTheme() noexcept {
        BOOL value = shouldUseDarkMode();

        DwmSetWindowAttribute(m_data->windowHandle, 
            static_cast<DWORD>(platform::win32::dwm::WindowAttribute::USE_IMMERSIVE_DARK_MODE), 
            &value,
            sizeof(value)
        );
    }

    void
    Win32Core::onDeactivate() noexcept {
        if (!m_data)
            return;

        for (auto &entry : m_data->keyboardKeyStates) {
            if (entry.second) {
                input::KeyReleasedEvent event{entry.first};
                onKeyReleased.invoke(event).displayErrorMessageBox();
            }

            entry.second = false;
        }
    }

    void 
    Win32Core::setDarkMode(DarkModeOption option) noexcept { 
        WindowAPI::setDarkMode(option);

        honorUserTheme();
    }

    base::Error
    Win32Core::setIcon(std::string_view fileName) noexcept {
        base::FunctionErrorGenerator errors{"WindowLibrary", "Win32Core"};
        
        int width, height, channelCount;
        std::string path{fileName};

        std::FILE *file;
        if (auto error = fopen_s(&file, path.c_str(), "rb"); error != 0) {
            errno = error;
            return errors.fromErrno("Open file");
        }

        unsigned char *data = stbi_load_from_file(file, &width, &height, &channelCount, STBI_default);
        
        if (data == nullptr) {
            return errors.error("Load data from file using stbi_load_from_file", stbi_failure_reason());
        }

        if (width < 1 || height < 1 || channelCount < 1)
            return errors.error("Load data from file using stbi", "incorrect parameters returned by stbi_load_from_file");

        m_data->iconData = IconData{data, std::uint32_t(width), std::uint32_t(height)};

        if (auto error = m_data->iconData->create())
            return error;

        auto icon = reinterpret_cast<LONG_PTR>(m_data->iconData->handle());

        SetClassLongPtr(m_data->windowHandle,
            GCLP_HICON, // changes icon
            icon);

        if (platform::win32::undocumented::setConsoleIcon(m_data->iconData->handle()))
            return base::Error::success();

        if (auto *consoleManager = platform::win32::ConsoleManager::instance()) {
            if (auto *windowHandle = static_cast<HWND>(consoleManager->findWindowHandle())) {
                if (SetClassLongPtr(windowHandle, GCLP_HICON, icon) != 0)
                    return base::Error::success();

                SendMessage(windowHandle, WM_SETICON, ICON_BIG, icon);
                SendMessage(windowHandle, WM_SETICON, ICON_SMALL, icon);
            }
        }

        return base::Error::success();
    }

    void 
    Win32Core::onVisibilityOptionUpdated() noexcept {
        switch (visibility()) { 
            case WindowVisibilityOption::HIDE:
                ShowWindow(m_data->windowHandle, SW_HIDE);
                break;
            case WindowVisibilityOption::SHOW:
                ShowWindow(m_data->windowHandle, SW_SHOWNORMAL);
                if (m_currentStandardCursor)
                    static_cast<void>(setCursor(m_currentStandardCursor.value()));
                break;
        }
    }

    void
    Win32Core::putMouseCursorAtCenterOfWindow() noexcept {
        m_data->previousMouseX = m_data->windowWidth / 2;
        m_data->previousMouseY = m_data->windowHeight / 2;

        POINT newCursorPosition{m_data->previousMouseX, m_data->previousMouseY};
        ClientToScreen(m_data->windowHandle, &newCursorPosition);
        
        assert(SetCursorPos(newCursorPosition.x, newCursorPosition.y));
    }

    [[nodiscard]] bool 
    Win32Core::shouldUseDarkMode() noexcept {
        switch (darkMode()) {
        case DarkModeOption::HONOR_USER_SETTINGS:
#ifdef LAVENDER_PLATFORM_UWP_AVAILABLE
            return platform::win32::uwp::getDarkModeSetting() == platform::win32::uwp::DarkMode::ON;
#else
            // TODO there is a function in some DLL
            return false;
#endif
        case DarkModeOption::FORCE_ON:
            return TRUE;
            break;
        case DarkModeOption::FORCE_OFF:
            return FALSE;
        }

        assert(false);
        return false;
    }

    const char *
    Win32Core::translateStandardCursorToCursorName(input::StandardCursor standardCursor) noexcept {
        switch (standardCursor) {
            case input::StandardCursor::ARROW:
                return IDC_ARROW;
            case input::StandardCursor::ARROW_WITH_HOURGLASS:
                    return IDC_APPSTARTING;
            case input::StandardCursor::HAND:
                return IDC_HAND;
            case input::StandardCursor::HELP:
                return IDC_HELP;
            case input::StandardCursor::HOURGLASS:
                return IDC_WAIT;
            case input::StandardCursor::I_BEAM:
                return IDC_IBEAM;
        }

        return nullptr;
    }


} // namespace window
