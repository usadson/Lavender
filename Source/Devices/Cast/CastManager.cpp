/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
*/

#define FMT_HEADER_ONLY

#include "CastManager.hpp"

#include <fmt/core.h>

#include "Source/Network/CommonSocketAddresses.hpp"

namespace devices::cast {
    
    CastManager::CastManager() noexcept = default;
    CastManager::~CastManager() noexcept = default;

    base::Error
    CastManager::initialize() noexcept {
#if 0
        base::Task::run(m_multiCastSocket.joinMultiCastGroup(network::common_socket_address::ssdpMulticast), {})
            ->then([&](std::any result) -> std::any {
                if (const auto error = std::any_cast<base::Error>(result)) {
                    error.displayErrorMessageBox();
                    return {};
                }

                fmt::print("[CAST] CONNECTED!!!!");

                return {};
            })
            ->then([&] (const std::any &) -> std::any {
                startUniCastSocket();
                return {};
            })
            ->then(m_multiCastSocket.write(
                "M-SEARCH * HTTP/1.1\r\n"
                "HOST: 239.255.255.250:1900\r\n"
                "MAN: \"ssdp:discover\"\r\n"
                "MX: 30\r\n"
                "ST: ssdp:all\r\n"
                "\r\n"))
            ->then([&](const std::any &param) -> std::any {
                if (auto error = std::any_cast<base::Error>(param)) {
                    error.displayErrorMessageBox();
                    return error;
                }

                while (true) {
                    TRY_GET_VARIABLE(data, m_multiCastSocket.readSynchronously())
                    fmt::print("DATA: {}\n", data);
                }

                return {};
            })
            ->then([&](const std::any &param) -> std::any {
                if (auto error = std::any_cast<base::Error>(param)) {
                    error.displayErrorMessageBox();
                    return error;
                }

                return {};
            })
        ;
#endif

        return base::Error::success();
    }

    void
    CastManager::startUniCastSocket() noexcept {
        base::Task::run(m_uniCastSocket.bind(network::SocketAddress{
            network::IPv4Address(192, 168, 1, 106),
            //m_multiCastSocket.multiCastInterface().value(),
            network::common_ports::ssdp
        }), {})
            ->then([&](const std::any &param) -> std::any {
                if (auto error = std::any_cast<base::Error>(param)) {
                    error.displayErrorMessageBox();
                    return error;
                }

                while (true) {
                    TRY_GET_VARIABLE(data, m_uniCastSocket.readSynchronously())
                    fmt::print("UNI DATA: {}\n", data);
                }

                return {};
            })
            ->then([&](const std::any& param) -> std::any {
                if (auto error = std::any_cast<base::Error>(param)) {
                    error.displayErrorMessageBox();
                    return error;
                }

                return {};
            });
    }

    
} // namespace devices::cast
