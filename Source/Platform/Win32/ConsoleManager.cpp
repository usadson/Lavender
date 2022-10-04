/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#define WIN32_LEAN_AND_MEAN

#include "ConsoleManager.hpp"

#include <array>

#ifndef NDEBUG
#   include <Windows.h>
#endif // !NDEBUG

#include <cassert>
#include <process.h>

namespace platform::win32 {

    static ConsoleManager *s_instance{nullptr};

    ConsoleManager::ConsoleManager() noexcept {
        assert(s_instance == nullptr);
        s_instance = this;
    }

    ConsoleManager *
    ConsoleManager::instance() noexcept {
        return s_instance;
    }
    
    ConsoleManager::~ConsoleManager() noexcept {
        if (s_instance == this)
            s_instance = nullptr;

        if (m_keepOpen)
            pause();

#ifndef NDEBUG
        if (m_stdin)
            std::fclose(m_stdin);

        if (m_stdout)
            std::fclose(m_stdout);

        if (m_stderr)
            std::fclose(m_stderr);

        FreeConsole();
#endif // !NDEBUG
    }
    
    void *
    ConsoleManager::findWindowHandle() const noexcept {
        static std::array<char, 1024> title{};
        auto length = GetConsoleTitleA(title.data(), static_cast<DWORD>(title.size()));
        if (length == 0)
            return nullptr;

        return FindWindow(nullptr, title.data());
    }

    bool
    ConsoleManager::initialize() noexcept {
#ifdef NDEBUG
        return true;
#else
        if (!AllocConsole())
            return false;

        if (freopen_s(&m_stdin, "conin$", "r", stdin) != 0)
            return false;

        if (freopen_s(&m_stdout, "conout$", "w", stdout) != 0)
            return false;

        if (freopen_s(&m_stderr, "conout$", "w", stderr) != 0)
            return false;

        return true;
#endif // !NDEBUG
    }

    void 
    ConsoleManager::pause() noexcept { 
        system("pause");
    }


} // namespace platform::win32
