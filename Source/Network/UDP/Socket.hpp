/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
*/

#pragma once

#ifndef _WIN32
#error "I am currently working on Windows, and only that platform is supported at the time! Sorry!"
#endif

#include "Base/Error.hpp"

#include <optional>
#include <string>
#include <string_view>

#include "Source/Base/ErrorOr.hpp"
#include "Source/Base/Task.hpp"
#include "Source/Network/SocketAddress.hpp"

namespace network::udp {

    class Socket {
    public:
        ~Socket() noexcept;

        [[nodiscard]] base::Task
        bind(SocketAddress) noexcept;

        [[nodiscard]] base::Task
        connect(SocketAddress remoteAddress) noexcept;

        [[nodiscard]] base::Task
        connect(SocketAddress remoteAddress, SocketAddress localAddress) noexcept;

        [[nodiscard]] base::Task
        joinMultiCastGroup(SocketAddress) noexcept;

        [[nodiscard]] base::Error
        leaveMultiCastGroup() noexcept;

        [[nodiscard]] constexpr std::optional<IPv4Address>
        multiCastInterface() const noexcept {
            return m_multiCastInterface;
        }

        [[nodiscard]] base::Task
        read(std::size_t bytes) noexcept;

        [[nodiscard]] base::ErrorOr<std::string>
        readSynchronously() noexcept;

        [[nodiscard]] base::Task
        write(std::string_view) noexcept;

    private:
        [[nodiscard]] base::Error
        doBind() noexcept;

        [[nodiscard]] base::Error
        doConnect() noexcept;

        [[nodiscard]] base::Error
        doCreateSocket() noexcept;

        [[nodiscard]] base::Error
        doDisableMultiCastLooping() noexcept;

        [[nodiscard]] base::Error
        doJoinMultiCastGroup() noexcept;

        [[nodiscard]] base::Error
        doSetMultiCastInterface() noexcept;

        std::optional<SocketAddress> m_remoteAddress;
        std::optional<SocketAddress> m_localAddress;

        std::optional<SocketAddress> m_multiCastGroup;
        std::optional<IPv4Address> m_multiCastInterface;

#ifdef _WIN32 
        std::uintptr_t m_socket{static_cast<std::uintptr_t>(~0)};
#else
        int m_multiCastSocket;
#endif
    };

} // namespace network::udp
