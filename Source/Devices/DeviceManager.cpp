/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
*/

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <processthreadsapi.h>
#endif // _WIN32

#include "DeviceManager.hpp"

#include <future>
#include <thread>

#include "Source/Devices/Cast/CastManager.hpp"

namespace devices {

    DeviceManager::DeviceManager() noexcept
            : m_castManager(std::make_unique<cast::CastManager>()){
    }

    DeviceManager::~DeviceManager() noexcept = default;

    base::Error
    DeviceManager::initialize() noexcept {
        const auto future = std::async(std::launch::async, [&] {
#ifdef _WIN32
            SetThreadDescription(GetCurrentThread(), L"devices::DeviceManager: CastManager runner");
#endif // _WIN32
            m_castManager->initialize().displayErrorMessageBox();
        });
        static_cast<void>(future);

        return base::Error::success();
    }
    
} // namespace devices
