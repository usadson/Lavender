/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#define WIN32_LEAN_AND_MEAN

#include "Source/Base/Error.hpp"

#include <array>
#include <sstream>

#include <cassert>
#include <cerrno>
#include <cstdio>

#include <Windows.h>

#ifdef min
#   undef min
#endif // min

#ifdef max
#   undef max
#endif // max

namespace base {

    thread_local static std::array<char, 512> s_descriptionBuffer{};

    std::string_view 
    FunctionErrorGenerator::errnoToErrorDescription() noexcept {
        switch (errno) {
            case 0:
                return "Success.";
            case EPERM:
                return "Operation not permitted.";
            case ENOENT:
                return "No such file or directory.";
            case ESRCH:
                return "No such process.";
            case EINTR:
                return "Interrupted system call.";
            case EIO:
                return "Input/output error.";
            case ENXIO:
                return "Device not configured.";
            case E2BIG:
                return "Argument list too long.";
            case ENOEXEC:
                return "Exec format error.";
            case EBADF:
                return "Bad file descriptor.";
            case ECHILD:
                return "No child processes.";
            case EAGAIN:
                return "Resource temporarily unavailable.";
            case ENOMEM:
                return "Cannot allocate memory.";
            case EACCES:
                return "File permission denied.";
            case EFAULT:
                return "Bad address.";
            case EBUSY:
                return "Device busy.";
            case EEXIST:
                return "File (already) exists.";
            case EXDEV:
                return "Cross-device link.";
            case ENODEV:
                return "Operation not supported by device.";
            case ENOTDIR:
                return "Not a directory.";
            case EISDIR:
                return "Cannot write to a directory.";
            case ENFILE:
                return "Too many open files in system.";
            case EMFILE:
                return "Too many open files in process.";
            case ENOTTY:
                return "Inappropriate ioctl for device.";
            case EFBIG:
                return "File too large."; 
            case ENOSPC:
                return "No space left on device.";
            case ESPIPE:
                return "Illegal seek."; 
            case EROFS:
                return "Cannot modify file or directory in a read-only filesystem.";
            case EMLINK:
                return "Too many links.";
            case EPIPE:
                return "Broken pipe.";
            case EDOM:
                return "Numerical argument out of domain."; 
            case EDEADLK:
                return "Resource deadlock avoided";
            case ENAMETOOLONG:
                return "File name too long.";    
            case ENOLCK:
                return "No locks available.";
            case ENOSYS:
                return "Function not implemented.";
            case ENOTEMPTY:
                 return "Directory not empty."; 
            default:
                strerror_s(s_descriptionBuffer.data(), s_descriptionBuffer.size(), errno);
                return s_descriptionBuffer.data();
        }
    }

    std::string_view
    FunctionErrorGenerator::winErrorToDescription(int error) noexcept {
        static int count{0};
        std::printf("[%d] Error: %lx\n", ++count, error);
        std::fflush(stdout);

        if (!error)
            return "Success";

        DWORD bufferLength = FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), s_descriptionBuffer.data(),
            static_cast<DWORD>(s_descriptionBuffer.size()), nullptr);

        std::printf("\tWinErr: %s\n", s_descriptionBuffer.data());

        if (bufferLength == 0 && GetLastError() == ERROR_MR_MID_NOT_FOUND) { 
            std::stringstream stream;
            stream << "Unknown error " << error << " (0x" << std::hex << error << ")";

            auto str = stream.str();
            strcpy_s(s_descriptionBuffer.data(), s_descriptionBuffer.size(), str.c_str());

            return s_descriptionBuffer.data();
        }

        error = GetLastError();
        assert(bufferLength > 0);
        
        return std::string_view{s_descriptionBuffer.data(), bufferLength};
    }

    std::string_view 
    FunctionErrorGenerator::winErrorToDescription() noexcept {
        return winErrorToDescription(GetLastError());
    }

} // namespace base
