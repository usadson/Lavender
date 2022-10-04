/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#ifndef LAVENDER_WIN32_SUPPORT_ENABLED
#error "Win32 support not enabled! This file shouldn't have been included."
#endif

#ifdef _WINCON_
#pragma message("<wincon.h> included with clashing macro's. Build errors expected.")
#undef MOUSE_MOVED
#endif

#define _WINCON_ // prevent wincon.h inclusion
#include <WinUser.h>

namespace platform::win32 {

    enum class WindowNotification : unsigned int {
#define PLATFORM_WIN32_REGISTER_WINDOW_NOTIFICATION(name, macro) name = macro,

#define PLATFORM_WIN32_ITERATE_WINDOW_NOTIFICATIONS(REGISTER_NOTIFICATION)                                             \
        REGISTER_NOTIFICATION(NONE, WM_NULL)                                                                               \
        REGISTER_NOTIFICATION(ACTIVATE, WM_ACTIVATE)                                                                       \
        REGISTER_NOTIFICATION(SET_FOCUS, WM_SETFOCUS)                                                                      \
        REGISTER_NOTIFICATION(KILL_FOCUS, WM_KILLFOCUS)                                                                    \
        REGISTER_NOTIFICATION(SET_REDRAW, WM_SETREDRAW)                                                                    \
        REGISTER_NOTIFICATION(SET_TEXT, WM_SETTEXT)                                                                        \
        REGISTER_NOTIFICATION(GET_TEXT, WM_GETTEXT)                                                                        \
        REGISTER_NOTIFICATION(GET_TEXT_LENGTH, WM_GETTEXTLENGTH)                                                           \
        REGISTER_NOTIFICATION(PAINT, WM_PAINT)                                                                             \
        REGISTER_NOTIFICATION(QUERY_END_SESSION, WM_QUERYENDSESSION)                                                       \
        REGISTER_NOTIFICATION(END_SESSION, WM_ENDSESSION)                                                                  \
        REGISTER_NOTIFICATION(ERASE_BACKGROUND, WM_ERASEBKGND)                                                             \
        REGISTER_NOTIFICATION(SYSTEM_COLOR_CHANGEd, WM_SYSCOLORCHANGE)                                                     \
        REGISTER_NOTIFICATION(WIN_INI_CHANGE, WM_WININICHANGE)                                                             \
        REGISTER_NOTIFICATION(SETTING_CHANGE, WM_SETTINGCHANGE)                                                            \
        REGISTER_NOTIFICATION(DEV_MODE_CHANGE, WM_DEVMODECHANGE)                                                           \
        REGISTER_NOTIFICATION(FONT_CHANGE, WM_FONTCHANGE)                                                                  \
        REGISTER_NOTIFICATION(TIME_CHANGE, WM_TIMECHANGE)                                                                  \
        REGISTER_NOTIFICATION(SET_CURSOR, WM_SETCURSOR)                                                                    \
        REGISTER_NOTIFICATION(MOUSE_ACTIVATE, WM_MOUSEACTIVATE)                                                            \
        REGISTER_NOTIFICATION(QUEUE_SYNC, WM_QUEUESYNC)                                                                    \
                                                                                                                           \
        REGISTER_NOTIFICATION(ACTIVATE_APP, WM_ACTIVATEAPP)                                                                \
        REGISTER_NOTIFICATION(CANCEL_MODE, WM_CANCELMODE)                                                                  \
        REGISTER_NOTIFICATION(CHILD_ACTIVATE, WM_CHILDACTIVATE)                                                            \
        REGISTER_NOTIFICATION(CLOSE, WM_CLOSE)                                                                             \
        REGISTER_NOTIFICATION(COMPACTING, WM_COMPACTING)                                                                   \
        REGISTER_NOTIFICATION(CREATE, WM_CREATE)                                                                           \
        REGISTER_NOTIFICATION(DESTROY, WM_DESTROY)                                                                         \
        REGISTER_NOTIFICATION(DPI_CHANGED, WM_DPICHANGED)                                                                  \
        REGISTER_NOTIFICATION(ENABLE, WM_ENABLE)                                                                           \
        REGISTER_NOTIFICATION(ENTER_SIZE_MOVE, WM_ENTERSIZEMOVE)                                                           \
        REGISTER_NOTIFICATION(EXIT_SIZE_MOVE, WM_EXITSIZEMOVE)                                                             \
        REGISTER_NOTIFICATION(GET_ICON, WM_GETICON)                                                                        \
        REGISTER_NOTIFICATION(GET_MIN_MAX_INFO, WM_GETMINMAXINFO)                                                          \
        REGISTER_NOTIFICATION(INPUT_LANGUAGE_CHANGE, WM_INPUTLANGCHANGE)                                                   \
        REGISTER_NOTIFICATION(INPUT_LANGUAGE_CHANGE_REQUEST, WM_INPUTLANGCHANGEREQUEST)                                    \
        REGISTER_NOTIFICATION(MOVE, WM_MOVE)                                                                               \
        REGISTER_NOTIFICATION(MOVING, WM_MOVING)                                                                           \
        REGISTER_NOTIFICATION(NO_OP, WM_NULL)                                                                              \
        REGISTER_NOTIFICATION(QUERY_DRAG_ICON, WM_QUERYDRAGICON)                                                           \
        REGISTER_NOTIFICATION(QUERY_OPEN, WM_QUERYOPEN)                                                                    \
        REGISTER_NOTIFICATION(QUIT, WM_QUIT)                                                                               \
        REGISTER_NOTIFICATION(SHOW_WINDOW, WM_SHOWWINDOW)                                                                  \
        REGISTER_NOTIFICATION(SIZE, WM_SIZE)                                                                               \
        REGISTER_NOTIFICATION(SIZING, WM_SIZING)                                                                           \
        REGISTER_NOTIFICATION(STYLE_CHANGED, WM_STYLECHANGED)                                                              \
        REGISTER_NOTIFICATION(STYLE_CHANGING, WM_STYLECHANGING)                                                            \
        REGISTER_NOTIFICATION(THEME_CHANGED, WM_THEMECHANGED)                                                              \
        REGISTER_NOTIFICATION(USER_CHANGED, WM_USERCHANGED)                                                                \
        REGISTER_NOTIFICATION(WINDOW_POSITION_CHANGED, WM_WINDOWPOSCHANGED)                                                \
        REGISTER_NOTIFICATION(WINDOW_POSITION_CHANGING, WM_WINDOWPOSCHANGING)                                              \
                                                                                                                           \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_CREATE, WM_NCCREATE)                                                         \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_DESTROY, WM_NCDESTROY)                                                       \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_CALCULATE_SIZE, WM_NCCALCSIZE)                                               \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_HIT_TEST, WM_NCHITTEST)                                                      \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_PAINT, WM_NCPAINT)                                                           \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_ACTIVATE, WM_NCACTIVATE)                                                     \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_GET_DLG_CODE, WM_GETDLGCODE)                                                 \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_SYNCRONIZE_PAINT, WM_SYNCPAINT)                                              \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_MOUSE_MOVE, WM_NCMOUSEMOVE)                                                  \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_MOUSE_LEFT_BUTTON_PRESSED, WM_NCLBUTTONDOWN)                                 \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_MOUSE_LEFT_BUTTON_RELEASED, WM_NCLBUTTONUP)                                  \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_MOUSE_LEFT_BUTTON_DOUBLE_CLICK, WM_NCLBUTTONDBLCLK)                          \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_MOUSE_RIGHT_BUTTON_, WM_NCRBUTTONDOWN)                                       \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_MOUSE_RIGHT_BUTTON_RELEASED, WM_NCRBUTTONUP)                                 \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_MOUSE_RIGHT_BUTTON_DOUBLE_CLICK, WM_NCRBUTTONDBLCLK)                         \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_MOUSE_MIDDLE_BUTTON_PRESSED, WM_NCMBUTTONDOWN)                               \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_MOUSE_MIDDLE_BUTTON_RELEASED, WM_NCMBUTTONUP)                                \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_MOUSE_MIDDLE_BUTTON_DOUBLE_CLICK, WM_NCMBUTTONDBLCLK)                        \
                                                                                                                           \
        REGISTER_NOTIFICATION(INPUT_METHOD_EDITOR_SET_CONTEXT, WM_IME_SETCONTEXT)                                          \
        REGISTER_NOTIFICATION(INPUT_METHOD_EDITOR_NOTIFY, WM_IME_NOTIFY)                                                   \
        REGISTER_NOTIFICATION(INPUT_METHOD_EDITOR_CONTROL, WM_IME_CONTROL)                                                 \
        REGISTER_NOTIFICATION(INPUT_METHOD_EDITOR_COMPOSITION_FULL, WM_IME_COMPOSITIONFULL)                                \
        REGISTER_NOTIFICATION(INPUT_METHOD_EDITOR_SELECT, WM_IME_SELECT)                                                   \
        REGISTER_NOTIFICATION(INPUT_METHOD_EDITOR_CHAR, WM_IME_CHAR)                                                       \
        REGISTER_NOTIFICATION(INPUT_METHOD_EDITOR_REQUEST, WM_IME_REQUEST)                                                 \
        REGISTER_NOTIFICATION(INPUT_METHOD_EDITOR_KEYDOWN, WM_IME_KEYDOWN)                                                 \
        REGISTER_NOTIFICATION(INPUT_METHOD_EDITOR_KEYUP, WM_IME_KEYUP)                                                     \
                                                                                                                           \
        REGISTER_NOTIFICATION(LOST_MOUSE_CAPTURE, WM_CAPTURECHANGED)                                                       \
                                                                                                                           \
        REGISTER_NOTIFICATION(MOUSE_MOVED, WM_MOUSEMOVE)                                                                   \
        REGISTER_NOTIFICATION(MOUSE_LEFT_BUTTON_PRESSED, WM_LBUTTONDOWN)                                                   \
        REGISTER_NOTIFICATION(MOUSE_LEFT_BUTTON_RELEASED, WM_LBUTTONUP)                                                    \
        REGISTER_NOTIFICATION(MOUSE_LEFT_BUTTON_DOUBLE_CLICK, WM_LBUTTONDBLCLK)                                            \
        REGISTER_NOTIFICATION(MOUSE_RIGHT_BUTTON_PRESSED, WM_RBUTTONDOWN)                                                  \
        REGISTER_NOTIFICATION(MOUSE_RIGHT_BUTTON_RELEASED, WM_RBUTTONUP)                                                   \
        REGISTER_NOTIFICATION(MOUSE_RIGHT_BUTTON_DOUBLE_CLICK, WM_RBUTTONDBLCLK)                                           \
        REGISTER_NOTIFICATION(MOUSE_MIDDLE_BUTTON_PRESSED, WM_MBUTTONDOWN)                                                 \
        REGISTER_NOTIFICATION(MOUSE_MIDDLE_BUTTON_RELEASED, WM_MBUTTONUP)                                                  \
        REGISTER_NOTIFICATION(MOUSE_MIDDLE_BUTTON_DOUBLE_CLICK, WM_MBUTTONDBLCLK)                                          \
        REGISTER_NOTIFICATION(MOUSE_WHEEL, WM_MOUSEWHEEL)                                                                  \
        REGISTER_NOTIFICATION(MOUSE_X_BUTTON_PRESSED, WM_XBUTTONDOWN)                                                      \
        REGISTER_NOTIFICATION(MOUSE_X_BUTTON_RELEASED, WM_XBUTTONUP)                                                       \
        REGISTER_NOTIFICATION(MOUSE_X_BUTTON_DOUBLE_CLICK, WM_XBUTTONDBLCLK)                                               \
        REGISTER_NOTIFICATION(MOUSE_HORIZONTAL_WHEEL, WM_MOUSEHWHEEL)                                                      \
                                                                                                                           \
        REGISTER_NOTIFICATION(SYSTEM_COMMAND, WM_SYSCOMMAND)                                                               \
        REGISTER_NOTIFICATION(DWM_COMPOSITION_CHANGED, WM_DWMCOMPOSITIONCHANGED)                                           \
        REGISTER_NOTIFICATION(DWM_NON_CLIENT_AREA_RENDERING_CHANGED, WM_DWMNCRENDERINGCHANGED)                             \
        REGISTER_NOTIFICATION(DWM_COLORIZATION_COLOR_CHANGED, WM_DWMCOLORIZATIONCOLORCHANGED)                              \
        REGISTER_NOTIFICATION(DWM_WINDOW_MAXIMIZED_CHANGE, WM_DWMWINDOWMAXIMIZEDCHANGE)                                    \
        REGISTER_NOTIFICATION(MOUSE_HOVER, WM_MOUSEHOVER)                                                                  \
        REGISTER_NOTIFICATION(MOUSE_LEAVE, WM_MOUSELEAVE)                                                                  \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_MOUSE_HOVER, WM_NCMOUSEHOVER)                                                \
        REGISTER_NOTIFICATION(NON_CLIENT_AREA_MOUSE_LEAVE, WM_NCMOUSELEAVE)                                                \
        REGISTER_NOTIFICATION(WTS_SESSION_CHANGE, WM_WTSSESSION_CHANGE)                                                    \
        REGISTER_NOTIFICATION(TABLET_FIRST, WM_TABLET_FIRST)                                                               \
        REGISTER_NOTIFICATION(TABLET_LAST, WM_TABLET_LAST)                                                                 \
        REGISTER_NOTIFICATION(CUT, WM_CUT)                                                                                 \
        REGISTER_NOTIFICATION(COPY, WM_COPY)                                                                               \
        REGISTER_NOTIFICATION(PASTE, WM_PASTE)                                                                             \
        REGISTER_NOTIFICATION(CLEAR, WM_CLEAR)                                                                             \
        REGISTER_NOTIFICATION(UNDO, WM_UNDO)                                                                               \
        REGISTER_NOTIFICATION(RENDER_FORMAT, WM_RENDERFORMAT)                                                              \
        REGISTER_NOTIFICATION(RENDER_ALL_FORMATS, WM_RENDERALLFORMATS)                                                     \
        REGISTER_NOTIFICATION(DESTORY_CLIPBOARD, WM_DESTROYCLIPBOARD)                                                      \
        REGISTER_NOTIFICATION(DRAW_CLIPBOARD, WM_DRAWCLIPBOARD)                                                            \
        REGISTER_NOTIFICATION(PAINT_CLIPBOARD, WM_PAINTCLIPBOARD)                                                          \
        REGISTER_NOTIFICATION(V_SCROLL_CLIPBOARD, WM_VSCROLLCLIPBOARD)                                                     \
        REGISTER_NOTIFICATION(SIZE_CLIPBOARD, WM_SIZECLIPBOARD)                                                            \
        REGISTER_NOTIFICATION(ASKCB_FORMAT_NAME, WM_ASKCBFORMATNAME)                                                       \
        REGISTER_NOTIFICATION(CHANGE_CB_CHAIN, WM_CHANGECBCHAIN)                                                           \
        REGISTER_NOTIFICATION(H_SCROLL_CLIPBOARD, WM_HSCROLLCLIPBOARD)                                                     \
        REGISTER_NOTIFICATION(QUERY_NEW_PALETTE, WM_QUERYNEWPALETTE)                                                       \
        REGISTER_NOTIFICATION(PALETTE_IS_CHANGING, WM_PALETTEISCHANGING)                                                   \
        REGISTER_NOTIFICATION(PALETTE_CHANGED, WM_PALETTECHANGED)                                                          \
        REGISTER_NOTIFICATION(HOTKEY, WM_HOTKEY)                                                                           \
        REGISTER_NOTIFICATION(PRINT, WM_PRINT)                                                                             \
        REGISTER_NOTIFICATION(PRINT_CLIENT, WM_PRINTCLIENT)                                                                \
        REGISTER_NOTIFICATION(APP_COMMAND, WM_APPCOMMAND)                                                                  \
        REGISTER_NOTIFICATION(CLIPBOARD_UPDATE, WM_CLIPBOARDUPDATE)                                                        \
        REGISTER_NOTIFICATION(DWM_SEND_ICON_IC_THUMBNAIL, WM_DWMSENDICONICTHUMBNAIL)                                       \
        REGISTER_NOTIFICATION(DWM_SEND_ICON_IC_LIVE_PREVIEW_BITMAP, WM_DWMSENDICONICLIVEPREVIEWBITMAP)                     \
        REGISTER_NOTIFICATION(GET_TITLEBAR_INFO_EX, WM_GETTITLEBARINFOEX)\
        REGISTER_NOTIFICATION(KEYBOARD_KEY_PRESSED, WM_KEYDOWN) \
        REGISTER_NOTIFICATION(KEYBOARD_KEY_RELEASED, WM_KEYUP) \
        REGISTER_NOTIFICATION(KEYBOARD_CHARACTER_TRANSLATED, WM_CHAR) \
        REGISTER_NOTIFICATION(KEYBOARD_DEAD_CHARACTER_TRANSLATED, WM_DEADCHAR) \
        REGISTER_NOTIFICATION(KEYBOARD_SYSTEM_KEY_PRESSED, WM_SYSKEYDOWN) \
        REGISTER_NOTIFICATION(KEYBOARD_SYSTEM_KEY_RELEASED, WM_SYSKEYUP) \
        REGISTER_NOTIFICATION(KEYBOARD_SYSTEM_CHARACTER_TRANSLATED, WM_SYSCHAR) \
        REGISTER_NOTIFICATION(KEYBOARD_SYSTEM_DEAD_CHARACTER_TRANSLATED, WM_SYSDEADCHAR) \


        //        REGISTER_NOTIFICATION(DPI_CHANGED_BEFORE_PARENT, WM_DPICHANGED_BEFOREPARENT)
        //        REGISTER_NOTIFICATION(DPI_CHANGED_AFTER_PARENT, WM_DPICHANGED_AFTERPARENT)
        //        REGISTER_NOTIFICATION(GET_DPI_SCALED_SIZE, WM_GETDPISCALEDSIZE)

        PLATFORM_WIN32_ITERATE_WINDOW_NOTIFICATIONS(PLATFORM_WIN32_REGISTER_WINDOW_NOTIFICATION)
#undef PLATFORM_WIN32_REGISTER_WINDOW_NOTIFICATION
    };

} // namespace platform::win32
