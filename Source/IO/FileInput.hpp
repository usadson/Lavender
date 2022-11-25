/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <iterator> // for std::distance
#include <string_view>
#include <vector>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#   define IO_FILEINPUT_USE_POSIX
#else
#   include <cstdio>
#endif

#include <cstddef> // for std::size_t

#include "Source/IO/Error.hpp"

namespace io {

    class FileInput {
#ifdef IO_FILEINPUT_USE_POSIX
        int m_file{};
#else
        std::FILE *m_file{};
#endif

        std::size_t m_size{};
        Error m_error{};

        /**
         * No return type, since errors are propagated through m_error.
         */
        void
        queryFileSize() noexcept;

    public:
        /**
         * Note: this function assumes the std::string_view's end is a NUL
         *       byte.
         */
        [[nodiscard]] FileInput(std::string_view fileName) noexcept;

        ~FileInput() noexcept;

        /**
         * Returns whether or not the file is valid.
         * 
         * This means that no error has occurred since the file has been opened
         * or when the file has been read from.
         */
        [[nodiscard]] inline constexpr
        operator bool() const noexcept {
            return m_error == Error::NO_ERROR;
        }

        [[nodiscard]] inline constexpr Error
        error() const noexcept {
            return m_error;
        }

        /**
         * Returns the size of the file.
         */
        [[nodiscard]] inline constexpr std::size_t
        size() const noexcept {
            return m_size;
        }

        /**
         * Reads from the file, into from base to base+size.
         * 
         * This function may fail, in that case returns false. The class-level
         * error member is set accordingly. In the case of an error, the data
         * from base to base+size is undefined.
         */
        [[nodiscard]] bool
        read(char *base, std::size_t size) noexcept;

        /**
         * Reads from the file, into from begin to end.
         *
         * This function may fail, in that case the class-level error member is
         * set accordingly. In the case of an error, the data range from begin
         * to end is undefined.
         */
        template <typename Iter>
        [[nodiscard]] inline bool
        read(Iter begin, Iter end) noexcept {
            const auto size = static_cast<std::size_t>(std::distance(begin, end));
            auto *base = &(*begin);

            if constexpr (std::is_same_v<decltype(base), char *>) {
                return read(base, size);
            } else {
                return read(reinterpret_cast<char *>(base), size);
            }
        }

        /**
         * Reads `size`-bytes from the file, stored in the return value vector.
         *
         * This function may fail, in that case the class-level error member is
         * set accordingly. If failed, the return value is the empty vector.
         */
        template <typename CharType = char8_t>
        [[nodiscard]] inline std::vector<CharType>
        read(std::size_t size) noexcept {
            std::vector<CharType> vec(size);

            if (!read(std::begin(vec), std::end(vec)))
                return {};

            return vec;
        }

    };

} // namespace base
