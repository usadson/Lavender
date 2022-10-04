/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <Windows.h>
#include <bluetoothapis.h>

namespace input::bluetooth::win32 {

    class BluetoothDeviceIteration {
    public:
        //[[nodiscard]] inline explicit
        //BluetoothDeviceIteration(HANDLE radio) noexcept {
        //    m_searchParams.hRadio = radio;
        //}

        inline 
        ~BluetoothDeviceIteration() noexcept {
            if (m_handle)
                BluetoothFindDeviceClose(m_handle);
        }

        template<typename Callback>
        inline void 
        run(Callback callback) {
            m_deviceInfo.dwSize = sizeof(m_deviceInfo);

            m_searchParams.dwSize = sizeof(m_searchParams);
            m_searchParams.cTimeoutMultiplier = 4;
            m_searchParams.fIssueInquiry = FALSE;
            m_searchParams.fReturnAuthenticated = TRUE;
            m_searchParams.fReturnConnected = TRUE;
            m_searchParams.fReturnRemembered = TRUE;
            m_searchParams.fReturnUnknown = TRUE;
            m_searchParams.hRadio = nullptr;

            m_handle = BluetoothFindFirstDevice(&m_searchParams, &m_deviceInfo);

            while (m_handle != nullptr) {
                callback(m_handle, m_deviceInfo);

                if (!next())
                    break;
            }
        }

    private:
        [[nodiscard]] bool 
        next() noexcept {
            return BluetoothFindNextDevice(m_handle, &m_deviceInfo);
        }

        BLUETOOTH_DEVICE_SEARCH_PARAMS m_searchParams{};
        BLUETOOTH_DEVICE_INFO m_deviceInfo{};
        HBLUETOOTH_DEVICE_FIND m_handle;
    };

} // namespace input::bluetooth::win32