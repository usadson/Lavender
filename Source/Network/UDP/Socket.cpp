/*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
*/

#define WIN32_LEAN_AND_MEAN
#include "Socket.hpp"

#include <thread>

#include "Base/ErrorOr.hpp"
#include "Source/Event/EventHandler.hpp"

#include <WinSock2.h>
#include <ws2ipdef.h>

#include <iphlpapi.h>
#include <iptypes.h>

#include "Source/Platform/Win32/WindowNotifications.hpp"

namespace platform::win32 {

    extern volatile HWND g_mainWindow;

} // namespace platform::win32

namespace network::udp {

    constexpr base::FunctionErrorGenerator errors{ "NetworkCore", "udp::Socket" };

    struct Deleter {
        void
        operator()(auto *ptr) {
            free(ptr);
        }
    };

    struct Adapter {
        IPv4Address ipAddress;
        IPv4Address gatewayAddress;
    };


    [[nodiscard]] IPv4Address
    stringToIPAddress(char data[16]) {
        std::size_t index = 0;

        auto parsePart = [&] {
            assert(index < 16);

            unsigned char value = data[index++] - '0';
            while (index < 16) {
                const auto character = data[index];
                if (character == '.' || character == '\0')
                    break;
                assert(character >= '0');
                assert(character <= '9');
                value *= 10;
                value += character - '0';
                index++;
            }

            return value;
        };

        const auto a = parsePart();
        ++index;
        const auto b = parsePart();
        ++index;
        const auto c = parsePart();
        ++index;
        const auto d = parsePart();
        return IPv4Address{ a, b, c, d };
    }

    [[nodiscard]] static base::ErrorOr<Adapter>
    findAdapter() {
        IP_ADAPTER_INFO stackAdapter;
        IP_ADAPTER_INFO *adapter = &stackAdapter;
        ULONG ulOutBufLen = sizeof stackAdapter;
        if (GetAdaptersInfo(adapter, &ulOutBufLen) != ERROR_SUCCESS) {
            adapter = static_cast<IP_ADAPTER_INFO*>(malloc(ulOutBufLen));
            assert(adapter);
        }

        ULONG dwRetVal;
        if ((dwRetVal = GetAdaptersInfo(adapter, &ulOutBufLen)) != ERROR_SUCCESS) {
            printf("GetAdaptersInfo call failed with %lu\n", dwRetVal);
            free(adapter);
            return errors.fromWinError("GetAdaptersInfo", dwRetVal);
        }

        PIP_ADAPTER_INFO pAdapter = adapter;
        while (pAdapter) {
            if (std::string_view{ pAdapter->GatewayList.IpAddress.String } != "0.0.0.0") {
                printf("Adapter Name: %s\n", pAdapter->AdapterName);
                printf("Adapter Desc: %s\n", pAdapter->Description);
                printf("\tAdapter Addr: \t");
                for (UINT i = 0; i < pAdapter->AddressLength; i++) {
                    if (i == (pAdapter->AddressLength - 1))
                        printf("%.2X\n", (int)pAdapter->Address[i]);
                    else
                        printf("%.2X-", (int)pAdapter->Address[i]);
                }
                printf("\tIP Address: %s\n", pAdapter->IpAddressList.IpAddress.String);
                printf("\tIP Mask: %s\n", pAdapter->IpAddressList.IpMask.String);
                printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
                printf("\t***\n");
                if (pAdapter->DhcpEnabled) {
                    printf("\tDHCP Enabled: Yes\n");
                    printf("\t\tDHCP Server: \t%s\n", pAdapter->DhcpServer.IpAddress.String);
                } else
                    printf("\tDHCP Enabled: No\n");

                Adapter result{
                     stringToIPAddress(pAdapter->IpAddressList.IpAddress.String),
                    stringToIPAddress(pAdapter->GatewayList.IpAddress.String)
                };

                free(adapter);
                return result;
            }

            pAdapter = pAdapter->Next;
        }

        free(adapter);
        return errors.error("Find Adapter", "No suitable adapter found.");
    }

    [[nodiscard]] static base::Error
    setReuseSocket(auto socket) noexcept {
        constexpr char enable = 1;
        if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof enable) < 0)
            return errors.fromWinError("setsockopt SO_REUSEADDR", WSAGetLastError());

        return base::Error::success();
    }

    Socket::~Socket() noexcept {
        if (m_socket != INVALID_SOCKET) {
            static_cast<void>(leaveMultiCastGroup());
            closesocket(m_socket);
        }
    }

    base::Task
    Socket::bind(SocketAddress address) noexcept {
        m_localAddress = address;

        return base::Task::create<base::Error>([this] {
            TRY(doCreateSocket())
            TRY(setReuseSocket(m_socket))
            TRY(doBind())
            return base::Error::success();
        });
    }

    base::Task
    Socket::connect(SocketAddress address) noexcept {
        return connect(address, SocketAddress{ IPv4Address{0, 0, 0, 0}, Port{0} });
    }

    base::Task
    Socket::connect(SocketAddress remoteAddress, SocketAddress localAddress) noexcept {
        m_remoteAddress = remoteAddress;
        m_localAddress = localAddress;
        
        return base::Task::create<base::Error>([this] {
            if (m_remoteAddress->port().value() == 0)
                return errors.error("Validate SocketAddress", "Port is 0");

            if (m_remoteAddress->address().asLong() == 0)
                return errors.error("Validate SocketAddress", "Address is 0");

            m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (m_socket == INVALID_SOCKET)
                return errors.fromWinError("Create TCP socket (WSA)", WSAGetLastError());

            TRY(setReuseSocket(m_socket))
            TRY(doBind())
            TRY(doConnect())
            return base::Error::success();
        });
    }

    base::Error
    Socket::doBind() noexcept {
        sockaddr_in addr{
            .sin_family = AF_INET,
            .sin_port = htons(m_localAddress->port().value()),
            .sin_addr = {
                .S_un = {
                    .S_addr = m_localAddress->address().asLong(),
                }
            }
        };

        std::printf("[Socket] Binding to: %hhu.%hhu.%hhu.%hhu:%hu\n", addr.sin_addr.S_un.S_un_b.s_b1, addr.sin_addr.S_un.S_un_b.s_b2, addr.sin_addr.S_un.S_un_b.s_b3, addr.sin_addr.S_un.S_un_b.s_b4, m_localAddress->port().value());

        if (::bind(m_socket, reinterpret_cast<const sockaddr*>(&addr), sizeof addr) == SOCKET_ERROR)
            return errors.fromWinError("Bind (WSA)", WSAGetLastError());

        auto size = static_cast<int>(sizeof addr);
        if (getsockname(m_socket, reinterpret_cast<sockaddr *>(&addr), &size) == SOCKET_ERROR)
            return errors.fromWinError("bind -> getsockname (WSA)", WSAGetLastError());

        m_localAddress = SocketAddress{ IPv4Address{addr.sin_addr.s_addr}, Port{ntohs(addr.sin_port)} };
        std::printf("[Socket] Actually bound to: %hhu.%hhu.%hhu.%hhu:%hu\n", m_localAddress->address()[0], m_localAddress->address()[1], m_localAddress->address()[2], m_localAddress->address()[3], m_localAddress->port().value());
        
        return base::Error::success();
    }

    base::Error
    Socket::doConnect() noexcept {
        sockaddr_in addr{
            .sin_family = AF_INET,
            .sin_port = htons(m_remoteAddress->port().value()),
            .sin_addr = {
                .S_un = {
                    .S_addr = m_remoteAddress->address().asLong(),
                }
            },
            .sin_zero = {},
        };

        if (::connect(m_socket, reinterpret_cast<const sockaddr*>(&addr), sizeof addr) == SOCKET_ERROR)
            return errors.fromWinError("Connect (WSA)", WSAGetLastError());

        return base::Error::success();
    }

    base::Error
    Socket::doCreateSocket() noexcept {
        m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_socket == INVALID_SOCKET)
            return errors.fromWinError("Create TCP socket (WSA)", WSAGetLastError());

        return base::Error::success();
    }

    base::Error
    Socket::doDisableMultiCastLooping() noexcept {
        char loopch = 0;

        if (setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_LOOP, &loopch, sizeof loopch) == SOCKET_ERROR)
            return errors.fromWinError("setsockopt IP_MULTICAST_LOOP", WSAGetLastError());

        return base::Error::success();
    }

    base::Error
    Socket::doJoinMultiCastGroup() noexcept {
        ip_mreq request;
        std::printf("[Socket] Joining MultiCast Group: %hhu.%hhu.%hhu.%hhu\n", m_multiCastGroup->address()[0], m_multiCastGroup->address()[1], m_multiCastGroup->address()[2], m_multiCastGroup->address()[3]);
        std::printf("[Socket] With Interface: %hhu.%hhu.%hhu.%hhu\n", (*m_multiCastInterface)[0], (*m_multiCastInterface)[1], (*m_multiCastInterface)[2], (*m_multiCastInterface)[3]);

        request.imr_multiaddr.s_addr = m_multiCastGroup->address().asLong();
        request.imr_interface.s_addr = m_multiCastInterface->asLong();
        auto ret = setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, static_cast<const char*>(static_cast<const void*>(&request)), sizeof request);
        if (ret == SOCKET_ERROR)
            return errors.fromWinError("setsockopt IP_ADD_MEMBERSHIP", WSAGetLastError());

        if (auto error = WSAGetLastError(); error != 0)
            return errors.fromWinError("Join MultiCast Group", error);

        return base::Error::success();
    }

    base::Error
    Socket::doSetMultiCastInterface() noexcept {
        TRY_GET_VARIABLE(adapter, findAdapter())

        const in_addr localInterface {
            .S_un{
                .S_addr = adapter.ipAddress.asLong()
            }
        };

        m_multiCastInterface = adapter.ipAddress;

        std::printf("[Socket] Set MultiCast interface address: %hhu.%hhu.%hhu.%hhu\n", adapter.ipAddress[0], adapter.ipAddress[1], adapter.ipAddress[2], adapter.ipAddress[3]);
        const auto ret = setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_IF, reinterpret_cast<const char *>(&localInterface), sizeof(localInterface));
        if (ret == SOCKET_ERROR)
            return errors.fromWinError("setsockopt IP_MULTICAST_IF", WSAGetLastError());
        return base::Error::success();
    }

    base::Task
    Socket::joinMultiCastGroup(SocketAddress address) noexcept {
        m_localAddress = {IPv4Address{0, 0, 0, 0}, address.port()};
        m_multiCastGroup = address;
        m_remoteAddress = address;

        return base::Task::create<base::Error>([this] {
            if (m_multiCastGroup->port().value() == 0)
                return errors.error("Validate SocketAddress", "Port is 0");

            if (m_multiCastGroup->address().asLong() == 0)
                return errors.error("Validate SocketAddress", "Address is 0");

            TRY(doCreateSocket())
            TRY(setReuseSocket(m_socket))
            TRY(doDisableMultiCastLooping())
            TRY(doSetMultiCastInterface())
            TRY(doBind())
            TRY(doJoinMultiCastGroup())
            return base::Error::success();
        });
    }

    base::Error
    Socket::leaveMultiCastGroup() noexcept {
        if (!m_multiCastGroup.has_value())
            return base::Error::success();

        ip_mreq request;
        request.imr_interface.s_addr = 0x0;
        request.imr_multiaddr.s_addr = m_multiCastGroup->address().asLong();
        auto ret = setsockopt(m_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, static_cast<const char*>(static_cast<const void*>(&request)), sizeof request);
        if (ret == SOCKET_ERROR)
            return errors.fromWinError("setsockopt IP_DROP_MEMBERSHIP", WSAGetLastError());

        m_multiCastGroup.reset();
        return base::Error::success();
    }

    base::Task
    Socket::read(const std::size_t bytes) noexcept {
        return base::Task::create<base::Error>([&] () -> base::ErrorOr<std::string> {
            std::string data(bytes, '\0');

            std::size_t index = 0;

            while (index != bytes) {
                const auto bytesRead = recv(m_socket, &data[index], static_cast<int>(bytes - index), 0);
                if (bytesRead == SOCKET_ERROR)
                    return errors.fromWinError("Read from TCP socket", WSAGetLastError());

                index += bytesRead;
            }

            return {std::move(data)};
        });
    }

    base::ErrorOr<std::string>
    Socket::readSynchronously() noexcept {
        std::string data(1024, '\0');

        sockaddr_in other{};
        int otherLength = static_cast<int>(sizeof other);
        
        const auto bytesRead = recvfrom(m_socket, data.data(), static_cast<int>(data.capacity()), 0, reinterpret_cast<sockaddr*>(&other), &otherLength);

        if (bytesRead == 0)
            return errors.error("Read from socket", "Socket is gracefully closed.");

        if (bytesRead == SOCKET_ERROR) {
            auto lastError = WSAGetLastError();
            return errors.fromWinError("Read from TCP socket", lastError);
        }

        data.resize(static_cast<std::size_t>(bytesRead));
        return {std::move(data)};
    }

    base::Task
    Socket::write(std::string_view data) noexcept {
        return base::Task::create<base::Error>([dataOwner = std::string(data), this]() -> base::Error {
            std::string_view data{dataOwner};
            sockaddr_in groupSock{
                .sin_family = AF_INET,
                .sin_port = htons(m_multiCastGroup->port().value()),
                .sin_addr = {
                    .S_un = {
                        .S_addr = m_multiCastGroup->address().asLong()
                    },
                },
            };

            while (data.length() > 0) {
                const auto bytesSent = sendto(m_socket, data.data(), static_cast<int>(data.length()), 0, reinterpret_cast<const sockaddr *>(&groupSock), static_cast<int>(sizeof groupSock));
                if (bytesSent == SOCKET_ERROR)
                    return errors.fromWinError("Send to TCP socket", WSAGetLastError());

                data = data.substr(static_cast<std::size_t>(bytesSent));
            }

            return base::Error::success();
        });
    }

} // namespace network::udp
