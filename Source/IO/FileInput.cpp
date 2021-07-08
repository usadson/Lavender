/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "FileInput.hpp"

#ifdef IO_FILEINPUT_USE_POSIX
#   include <sys/stat.h>
#   include <fcntl.h>
#   include <unistd.h>
#endif

#include <cassert>
#include <cerrno>

namespace io {

    [[nodiscard]] inline constexpr Error
    associateOpenErrnoWithIOError(int error) noexcept {
        switch (error) {
            case ENOENT:
                return Error::FILE_NOT_FOUND;
            case EFAULT:
            case EISDIR:
            case ENAMETOOLONG:
                return Error::INVALID_FILE_NAME;
            case EACCES:
                return Error::ACCESS_DENIED;
            default:
                return Error::UNKNOWN_OPEN_ERROR;
        }
    }

    [[nodiscard]] inline constexpr Error
    associateReadErrnoWithIOError(int error) noexcept {
        return Error::UNKNOWN_READ_ERROR;
    }

    FileInput::FileInput(std::string_view name) noexcept {
#ifdef IO_FILEINPUT_USE_POSIX
        m_file = open(std::data(name), O_RDONLY);

        if (m_file == -1) {
            m_error = associateOpenErrnoWithIOError(errno);
            return;
        }
#else
        const auto error = fopen_s(&m_file, std::data(name), "rb");
        if (error != 0) {
            m_error = associateOpenErrnoWithIOError(error);
            return;
        }
#endif

        queryFileSize();
    }

    FileInput::~FileInput() noexcept {
#ifdef IO_FILEINPUT_USE_POSIX
        if (m_file >= 0)
            static_cast<void>(close(m_file));
#else
        if (m_file)
            static_cast<void>(fclose(m_file));
#endif
    }

    void
    FileInput::queryFileSize() noexcept {
#ifdef IO_FILEINPUT_USE_POSIX
        struct stat status{};

        if (fstat(m_file, &status) == -1) {
            m_error = Error::POSIX_STAT_ERROR;

            if (m_file >= 0) {
                close(m_file);
                m_file = -1;
            }

            return;
        }

        m_size = static_cast<std::size_t>(status.st_size);
#else
        const auto prevPos = ftell(m_file);

        if (std::fseek(m_file, 0, SEEK_END) != 0) {
            m_error = Error::CFILE_SEEK_ERROR;
            return;
        }

        const auto size = std::ftell(m_file);
        if (size == -1) {
            m_error = Error::CFILE_TELL_ERROR;
            return;
        }

        assert(size >= 0);

        if (std::fseek(m_file, prevPos, SEEK_SET) != 0) {
            m_error = Error::CFILE_SEEK_ERROR;
            return;
        }

        m_size = static_cast<std::size_t>(size);
#endif
    }

    bool
    FileInput::read(char *data, std::size_t size) noexcept {
        while (size != 0) {
#ifdef IO_FILEINPUT_USE_POSIX
            const auto ret = ::read(m_file, data, size);

            if (ret == 0) {
                m_error = Error::TRY_READ_PAST_EOF;
                return false;
            }

            if (ret == -1) {
                m_error = associateReadErrnoWithIOError(errno);
                return false;
            }
#else
            const auto ret = fread(data, 1, size, m_file);

            if (ret == 0) {
                m_error = feof(m_file)
                    ? Error::TRY_READ_PAST_EOF
                    : associateReadErrnoWithIOError(errno);

                return false;
            }
#endif

            data += ret;
            size -= ret;
        }

        return true;
    }

} // namespace io
