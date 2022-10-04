/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#include "Source/Input/Bluetooth/Win32BluetoothManager.hpp"

#include <iostream>
#include <string>
#include <thread>

#include <cassert>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ws2bth.h>
#include <cguid.h>
#include <winsock2.h>

#include "Win32BluetoothDeviceIteration.hpp"

namespace input::bluetooth {

    Win32BluetoothManager::~Win32BluetoothManager() noexcept = default;

    struct Device {
        ~Device() noexcept { 
            if (m_socket > 0)
                closesocket(m_socket);
        }

        static void 
        printResult(int error) {
            switch (error) {
            case ERROR_SUCCESS:
                std::cout << "Success.\n";
                break;
            case ERROR_CANCELLED:
                std::cout << "Cancelled.\n";
                break;
            case ERROR_INVALID_PARAMETER:
                std::cout << "Invalid Parameter.\n";
                break;
            case ERROR_NO_MORE_ITEMS:
                std::cout << "No more items.\n";
                break;
            case E_FAIL:
                std::cout << "Failed.\n";
                break;
            case ERROR_NOT_AUTHENTICATED:
                std::cout << "Not authenticated.\n";
                break;
            default:
                std::cout << "Unknown error: " << error << "\n";
                break;
            }
        }

        [[nodiscard]] base::Error
        initialize(const BLUETOOTH_DEVICE_INFO &deviceInfo) {
            base::FunctionErrorGenerator errors{"InputLibrary", "Win32BluetoothManager/Device"};

            auto info = deviceInfo;

            if (!deviceInfo.fAuthenticated) {
                HBLUETOOTH_AUTHENTICATION_REGISTRATION registration;
                auto registrationError = BluetoothRegisterForAuthenticationEx(
                    &deviceInfo, 
                    &registration,
                    // typedef BOOL (CALLBACK *PFN_AUTHENTICATION_CALLBACK_EX)(_In_opt_ LPVOID pvParam, _In_ PBLUETOOTH_AUTHENTICATION_CALLBACK_PARAMS pAuthCallbackParams);
                    [](LPVOID pvParam, PBLUETOOTH_AUTHENTICATION_CALLBACK_PARAMS pAuthCallbackParams) -> BOOL {
                        auto *device = static_cast<Device *>(pvParam);
                        std::cout << "Authetncatab!\n";

                        BLUETOOTH_AUTHENTICATE_RESPONSE response{};
                        response.bthAddressRemote = pAuthCallbackParams->deviceInfo.Address;
                        response.authMethod = pAuthCallbackParams->authenticationMethod;
                        for (std::size_t i = 0; i < 6; ++i)
                            response.pinInfo.pin[i] = pAuthCallbackParams->deviceInfo.Address.rgBytes[5 - i];
                        response.pinInfo.pinLength = 6;
                        response.negativeResponse = FALSE;

                        std::cout << "PIN: " << std::hex;
                        for (std::size_t i = 0; i < 6; ++i)
                            std::cout << static_cast<int>(response.pinInfo.pin[i]) << " ";
                        std::cout << "\n" << std::dec;

                        auto error = BluetoothSendAuthenticationResponseEx(nullptr, &response);
                        std::cout << "Sending response: ";
                        printResult(error);
                        return true;
                    },
                    this
                );

                std::cout << "Registration went: ";
                printResult(registrationError);

                auto error = BluetoothAuthenticateDeviceEx(
                    nullptr, 
                    nullptr, 
                    &info, 
                    nullptr, 
                    MITMProtectionNotRequired
                );
                std::cout << "Authenticate: ";
                printResult(error);
            }

            m_socket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
            if (m_socket == INVALID_SOCKET)
                return errors.fromErrno("Create AF_BTH socket");

            SOCKADDR_BTH socketAddress{};
            socketAddress.addressFamily = AF_BTH;
            socketAddress.btAddr = deviceInfo.Address.ullLong;
            
            // 4d36e972
            // e325
            // 11ce
            // bfc1
            // 08002be10318
            socketAddress.serviceClassId.Data1 = 0x4d36e972;
            socketAddress.serviceClassId.Data2 = 0xe325;
            socketAddress.serviceClassId.Data3 = 0x11ce;
            socketAddress.serviceClassId.Data4[0] = 0xbfc1;
            socketAddress.serviceClassId.Data4[1] = 0x0800;
            socketAddress.serviceClassId.Data4[2] = 0x2be1;
            socketAddress.serviceClassId.Data4[3] = 0x0318;
            //socketAddress.port = 0x11;

            if (connect(m_socket, reinterpret_cast<const sockaddr *>(&socketAddress), sizeof(socketAddress)) == SOCKET_ERROR) {
                return errors.fromWinError("Connect to Bluetooth Socket", WSAGetLastError());
            }

            errors.error("", "Success").displayErrorMessageBox();
            return base::Error::success();
        }

    private:
        int m_socket{-1};
    };

    void
    handleDevice(BLUETOOTH_DEVICE_INFO deviceInfo) {
        Device device;
        if (auto error = device.initialize(deviceInfo)) {
            error.displayErrorMessageBox();
            return;
        }
    }

    base::Error
    Win32BluetoothManager::initialize() noexcept {
        base::FunctionErrorGenerator generator{"InputLibrary", "Win32BluetoothManager"};

        WSADATA wsaData{};
        auto wsaVersionRequest = MAKEWORD(2, 2);
        if (auto err = WSAStartup(wsaVersionRequest, &wsaData); err != 0) {
            return generator.fromWinError("Initialize WSA", err);
        }

        std::thread thread([&]() {
            win32::BluetoothDeviceIteration iteration{};
            iteration.run([&](HBLUETOOTH_DEVICE_FIND, const BLUETOOTH_DEVICE_INFO &deviceInfo) {
                std::wcout << L"[Input/Bluetooth] (Win32/Initialize): Found device: " << deviceInfo.szName << " "
                           << deviceInfo.ulClassofDevice << "\n";

                std::wstring_view name{deviceInfo.szName};
                if (name == L"Nintendo RVL-CNT-01") {
                    handleDevice(deviceInfo);
                } else
                    std::cout << "Device mismatch!\n";
            });
        });
        thread.detach();

        //return generator.fromWinError("do stuff");
        return base::Error::success();
    }

} // namespace input::bluetooth