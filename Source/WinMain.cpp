/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
*/

#ifndef UNICODE
#   define UNICODE
#endif
#define WIN32_LEAN_AND_MEAN
#define _WINCON_ // prevent wincon.h inclusion
#include <windows.h>

#include <cstdio>
#include <cstdlib>

#include <filesystem>
#include <iostream>

#include "Lavender.hpp"
#include "Source/Platform/WinPlatform.hpp"
#include "Source/Platform/Win32/ConsoleManager.hpp"

int WINAPI WinMain(HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance,
                   [[maybe_unused]] LPSTR pCmdLine, [[maybe_unused]] int nCmdShow) {
    platform::win32::g_hInstance = hInstance;

    std::setlocale(LC_ALL, "");

    platform::win32::ConsoleManager consoleManager{};
    consoleManager.initialize();
    consoleManager.setKeepOpen(true);

    auto path = std::filesystem::current_path();
    while (true) {
        if (std::filesystem::exists(path / "Resources/Lavender.ini")) {
            std::filesystem::current_path(path);
            break;
        }

        if (!path.has_parent_path())
            break;

        path = path.parent_path();
    }

    Lavender lavender{};

    const auto status = lavender.run();
    if (status == base::ExitStatus::SUCCESS)
        return EXIT_SUCCESS;

    std::printf("Lavender: ExitStatus is %s (%zu)\n",
        base::translateExitStatusToStringView(status).data(),
        static_cast<std::size_t>(status));
    return EXIT_FAILURE;
}
