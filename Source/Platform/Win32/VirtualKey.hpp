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

#define _WINCON_ // prevent wincon.h inclusion
#include <WinUser.h>

#ifdef DELETE
// Warning: certain macros are deleted that clash with enums in input::KeyboardKey
#undef DELETE
#endif

namespace platform::win32 {

    enum class VirtualKey : std::uint8_t {

#define PLATFORM_WIN32_ITERATE_VIRTUAL_KEYS(REGISTER_VIRTUAL_KEY) \
        REGISTER_VIRTUAL_KEY(LEFT_MOUSE_BUTTON, VK_LBUTTON, std::nullopt) \
        REGISTER_VIRTUAL_KEY(RIGHT_MOUSE_BUTTON, VK_RBUTTON, std::nullopt) \
        REGISTER_VIRTUAL_KEY(CANCEL, VK_CANCEL, std::nullopt) \
        REGISTER_VIRTUAL_KEY(MIDDLE_MOUSE_BUTTON, VK_MBUTTON, std::nullopt) \
        REGISTER_VIRTUAL_KEY(X1_MOUSE_BUTTON, VK_XBUTTON1, std::nullopt) \
        REGISTER_VIRTUAL_KEY(X2_MOUSE_BUTTON, VK_XBUTTON2, std::nullopt) \
        REGISTER_VIRTUAL_KEY(BACKSPACE, VK_BACK, input::KeyboardKey::BACKSPACE) \
        REGISTER_VIRTUAL_KEY(TAB, VK_TAB, input::KeyboardKey::TAB) \
        REGISTER_VIRTUAL_KEY(CLEAR, VK_CLEAR, std::nullopt) \
        REGISTER_VIRTUAL_KEY(CONTROL, VK_CONTROL, std::nullopt) \
        REGISTER_VIRTUAL_KEY(MENU, VK_MENU, std::nullopt) \
        REGISTER_VIRTUAL_KEY(PAUSE, VK_PAUSE, std::nullopt) \
        REGISTER_VIRTUAL_KEY(CAPS_LOCK, VK_CAPITAL, input::KeyboardKey::CAPS_LOCK) \
        REGISTER_VIRTUAL_KEY(IME_MODE_KANA, VK_KANA, std::nullopt) \
        REGISTER_VIRTUAL_KEY(IME_ENABLE, VK_IME_ON, std::nullopt) \
        REGISTER_VIRTUAL_KEY(IME_MODE_JUNJA, VK_JUNJA, std::nullopt) \
        REGISTER_VIRTUAL_KEY(IME_MODE_FINAL, VK_FINAL, std::nullopt) \
        REGISTER_VIRTUAL_KEY(IME_MODE_KANJI, VK_KANJI, std::nullopt) \
        REGISTER_VIRTUAL_KEY(IME_DISABLE, VK_IME_OFF, std::nullopt) \
        REGISTER_VIRTUAL_KEY(ESCAPE, VK_ESCAPE, input::KeyboardKey::ESCAPE) \
        REGISTER_VIRTUAL_KEY(IME_CONVERT, VK_CONVERT, std::nullopt) \
        REGISTER_VIRTUAL_KEY(IME_NONCONVERT, VK_NONCONVERT, std::nullopt) \
        REGISTER_VIRTUAL_KEY(IME_ACCEPT, VK_ACCEPT, std::nullopt) \
        REGISTER_VIRTUAL_KEY(IME_CHANGE_MODE, VK_MODECHANGE, std::nullopt) \
        REGISTER_VIRTUAL_KEY(SPACE, VK_SPACE, input::KeyboardKey::SPACE) \
        REGISTER_VIRTUAL_KEY(PAGE_UP, VK_PRIOR, input::KeyboardKey::PAGE_UP) \
        REGISTER_VIRTUAL_KEY(PAGE_DOWN, VK_NEXT, input::KeyboardKey::PAGE_DOWN) \
        REGISTER_VIRTUAL_KEY(END, VK_END, input::KeyboardKey::END) \
        REGISTER_VIRTUAL_KEY(HOME, VK_HOME, input::KeyboardKey::HOME) \
        REGISTER_VIRTUAL_KEY(ARROW_LEFT, VK_LEFT, input::KeyboardKey::ARROW_LEFT) \
        REGISTER_VIRTUAL_KEY(ARROW_UP, VK_UP, input::KeyboardKey::ARROW_UP) \
        REGISTER_VIRTUAL_KEY(ARROW_RIGHT, VK_RIGHT, input::KeyboardKey::ARROW_RIGHT) \
        REGISTER_VIRTUAL_KEY(ARROW_DOWN, VK_DOWN, input::KeyboardKey::ARROW_DOWN) \
        REGISTER_VIRTUAL_KEY(SELECT, VK_SELECT, input::KeyboardKey::SELECT) \
        REGISTER_VIRTUAL_KEY(PRINT, VK_PRINT, input::KeyboardKey::PRINT) \
        REGISTER_VIRTUAL_KEY(EXECUTE, VK_EXECUTE, std::nullopt) \
        REGISTER_VIRTUAL_KEY(PRINT_SCREEN, VK_SNAPSHOT, input::KeyboardKey::PRINT_SCREEN) \
        REGISTER_VIRTUAL_KEY(INSERT, VK_INSERT, input::KeyboardKey::INSERT) \
        REGISTER_VIRTUAL_KEY(DELETE, VK_DELETE, input::KeyboardKey::DELETE) \
        REGISTER_VIRTUAL_KEY(HELP, VK_HELP, input::KeyboardKey::HELP) \
        REGISTER_VIRTUAL_KEY(DIGIT0, 0x30, input::KeyboardKey::DIGIT0) \
        REGISTER_VIRTUAL_KEY(DIGIT1, 0x31, input::KeyboardKey::DIGIT1) \
        REGISTER_VIRTUAL_KEY(DIGIT2, 0x32, input::KeyboardKey::DIGIT2) \
        REGISTER_VIRTUAL_KEY(DIGIT3, 0x33, input::KeyboardKey::DIGIT3) \
        REGISTER_VIRTUAL_KEY(DIGIT4, 0x34, input::KeyboardKey::DIGIT4) \
        REGISTER_VIRTUAL_KEY(DIGIT5, 0x35, input::KeyboardKey::DIGIT5) \
        REGISTER_VIRTUAL_KEY(DIGIT6, 0x36, input::KeyboardKey::DIGIT6) \
        REGISTER_VIRTUAL_KEY(DIGIT7, 0x37, input::KeyboardKey::DIGIT7) \
        REGISTER_VIRTUAL_KEY(DIGIT8, 0x38, input::KeyboardKey::DIGIT8) \
        REGISTER_VIRTUAL_KEY(DIGIT9, 0x39, input::KeyboardKey::DIGIT9) \
        REGISTER_VIRTUAL_KEY(A, 0x41, input::KeyboardKey::A) \
        REGISTER_VIRTUAL_KEY(B, 0x42, input::KeyboardKey::B) \
        REGISTER_VIRTUAL_KEY(C, 0x43, input::KeyboardKey::C) \
        REGISTER_VIRTUAL_KEY(D, 0x44, input::KeyboardKey::D) \
        REGISTER_VIRTUAL_KEY(E, 0x45, input::KeyboardKey::E) \
        REGISTER_VIRTUAL_KEY(F, 0x46, input::KeyboardKey::F) \
        REGISTER_VIRTUAL_KEY(G, 0x47, input::KeyboardKey::G) \
        REGISTER_VIRTUAL_KEY(H, 0x48, input::KeyboardKey::H) \
        REGISTER_VIRTUAL_KEY(I, 0x49, input::KeyboardKey::I) \
        REGISTER_VIRTUAL_KEY(J, 0x4A, input::KeyboardKey::J) \
        REGISTER_VIRTUAL_KEY(K, 0x4B, input::KeyboardKey::K) \
        REGISTER_VIRTUAL_KEY(L, 0x4C, input::KeyboardKey::L) \
        REGISTER_VIRTUAL_KEY(M, 0x4D, input::KeyboardKey::M) \
        REGISTER_VIRTUAL_KEY(N, 0x4E, input::KeyboardKey::N) \
        REGISTER_VIRTUAL_KEY(O, 0x4F, input::KeyboardKey::O) \
        REGISTER_VIRTUAL_KEY(P, 0x50, input::KeyboardKey::P) \
        REGISTER_VIRTUAL_KEY(Q, 0x51, input::KeyboardKey::Q) \
        REGISTER_VIRTUAL_KEY(R, 0x52, input::KeyboardKey::R) \
        REGISTER_VIRTUAL_KEY(S, 0x53, input::KeyboardKey::S) \
        REGISTER_VIRTUAL_KEY(T, 0x54, input::KeyboardKey::T) \
        REGISTER_VIRTUAL_KEY(U, 0x55, input::KeyboardKey::U) \
        REGISTER_VIRTUAL_KEY(V, 0x56, input::KeyboardKey::V) \
        REGISTER_VIRTUAL_KEY(W, 0x57, input::KeyboardKey::W) \
        REGISTER_VIRTUAL_KEY(X, 0x58, input::KeyboardKey::X) \
        REGISTER_VIRTUAL_KEY(Y, 0x59, input::KeyboardKey::Y) \
        REGISTER_VIRTUAL_KEY(Z, 0x5A, input::KeyboardKey::Z) \
        REGISTER_VIRTUAL_KEY(LEFT_META, VK_LWIN, input::KeyboardKey::LEFT_META) \
        REGISTER_VIRTUAL_KEY(RIGHT_META, VK_RWIN, input::KeyboardKey::RIGHT_META) \
        REGISTER_VIRTUAL_KEY(APPS, VK_APPS, std::nullopt) \
        REGISTER_VIRTUAL_KEY(SLEEP, VK_SLEEP, std::nullopt) \
        REGISTER_VIRTUAL_KEY(NUMPAD0, 0x60, input::KeyboardKey::NUMPAD0) \
        REGISTER_VIRTUAL_KEY(NUMPAD1, 0x61, input::KeyboardKey::NUMPAD1) \
        REGISTER_VIRTUAL_KEY(NUMPAD2, 0x62, input::KeyboardKey::NUMPAD2) \
        REGISTER_VIRTUAL_KEY(NUMPAD3, 0x63, input::KeyboardKey::NUMPAD3) \
        REGISTER_VIRTUAL_KEY(NUMPAD4, 0x64, input::KeyboardKey::NUMPAD4) \
        REGISTER_VIRTUAL_KEY(NUMPAD5, 0x65, input::KeyboardKey::NUMPAD5) \
        REGISTER_VIRTUAL_KEY(NUMPAD6, 0x66, input::KeyboardKey::NUMPAD6) \
        REGISTER_VIRTUAL_KEY(NUMPAD7, 0x67, input::KeyboardKey::NUMPAD7) \
        REGISTER_VIRTUAL_KEY(NUMPAD8, 0x68, input::KeyboardKey::NUMPAD8) \
        REGISTER_VIRTUAL_KEY(NUMPAD9, 0x69, input::KeyboardKey::NUMPAD9) \
        REGISTER_VIRTUAL_KEY(MULTIPLY, VK_MULTIPLY, input::KeyboardKey::NUMPAD_MULTIPLY) \
        REGISTER_VIRTUAL_KEY(ADD, VK_ADD, input::KeyboardKey::NUMPAD_PLUS) \
        REGISTER_VIRTUAL_KEY(SEPARATOR, VK_SEPARATOR, std::nullopt) \
        REGISTER_VIRTUAL_KEY(SUBTRACT, VK_SUBTRACT, input::KeyboardKey::NUMPAD_MINUS) \
        REGISTER_VIRTUAL_KEY(DECIMAL, VK_DECIMAL, input::KeyboardKey::NUMPAD_DECIMAL) \
        REGISTER_VIRTUAL_KEY(DIVIDE, VK_DIVIDE, input::KeyboardKey::NUMPAD_DIVIDE) \
        REGISTER_VIRTUAL_KEY(F1, VK_F1, input::KeyboardKey::F1) \
        REGISTER_VIRTUAL_KEY(F2, VK_F2, input::KeyboardKey::F2) \
        REGISTER_VIRTUAL_KEY(F3, VK_F3, input::KeyboardKey::F3) \
        REGISTER_VIRTUAL_KEY(F4, VK_F4, input::KeyboardKey::F4) \
        REGISTER_VIRTUAL_KEY(F5, VK_F5, input::KeyboardKey::F5) \
        REGISTER_VIRTUAL_KEY(F6, VK_F6, input::KeyboardKey::F6) \
        REGISTER_VIRTUAL_KEY(F7, VK_F7, input::KeyboardKey::F7) \
        REGISTER_VIRTUAL_KEY(F8, VK_F8, input::KeyboardKey::F8) \
        REGISTER_VIRTUAL_KEY(F9, VK_F9, input::KeyboardKey::F9) \
        REGISTER_VIRTUAL_KEY(F10, VK_F10, input::KeyboardKey::F10) \
        REGISTER_VIRTUAL_KEY(F11, VK_F11, input::KeyboardKey::F11) \
        REGISTER_VIRTUAL_KEY(F12, VK_F12, input::KeyboardKey::F12) \
        REGISTER_VIRTUAL_KEY(F13, VK_F13, input::KeyboardKey::F13) \
        REGISTER_VIRTUAL_KEY(F14, VK_F14, input::KeyboardKey::F14) \
        REGISTER_VIRTUAL_KEY(F15, VK_F15, input::KeyboardKey::F15) \
        REGISTER_VIRTUAL_KEY(F16, VK_F16, input::KeyboardKey::F16) \
        REGISTER_VIRTUAL_KEY(F17, VK_F17, input::KeyboardKey::F17) \
        REGISTER_VIRTUAL_KEY(F18, VK_F18, input::KeyboardKey::F18) \
        REGISTER_VIRTUAL_KEY(F19, VK_F19, input::KeyboardKey::F19) \
        REGISTER_VIRTUAL_KEY(F20, VK_F20, input::KeyboardKey::F20) \
        REGISTER_VIRTUAL_KEY(F21, VK_F21, input::KeyboardKey::F21) \
        REGISTER_VIRTUAL_KEY(F22, VK_F22, input::KeyboardKey::F22) \
        REGISTER_VIRTUAL_KEY(F23, VK_F23, input::KeyboardKey::F23) \
        REGISTER_VIRTUAL_KEY(F24, VK_F24, input::KeyboardKey::F24) \
        REGISTER_VIRTUAL_KEY(NUMLOCK, VK_NUMLOCK, input::KeyboardKey::NUMPAD_LOCK) \
        REGISTER_VIRTUAL_KEY(LSHIFT, VK_LSHIFT, input::KeyboardKey::LEFT_SHIFT) \
        REGISTER_VIRTUAL_KEY(RSHIFT, VK_RSHIFT, input::KeyboardKey::RIGHT_SHIFT) \
        REGISTER_VIRTUAL_KEY(LCONTROL, VK_LCONTROL, input::KeyboardKey::LEFT_CONTROL) \
        REGISTER_VIRTUAL_KEY(RCONTROL, VK_RCONTROL, input::KeyboardKey::RIGHT_CONTROL) \
        REGISTER_VIRTUAL_KEY(LALT, VK_LMENU, input::KeyboardKey::LEFT_ALT) \
        REGISTER_VIRTUAL_KEY(RALT, VK_RMENU, input::KeyboardKey::RIGHT_ALT) \
        
        // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes


#define PLATFORM_WIN32_REGISTER_VIRTUAL_KEY(name, macro, inputKey) name = macro,
        PLATFORM_WIN32_ITERATE_VIRTUAL_KEYS(PLATFORM_WIN32_REGISTER_VIRTUAL_KEY)
#undef PLATFORM_WIN32_REGISTER_VIRTUAL_KEY
    };

} // namespace platform::win32
