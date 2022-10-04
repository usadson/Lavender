/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <string_view>

namespace io {

    enum class Error {

#define IO_IGNORE_SECTION_ANNOUNCEMENTS(s)

#define IO_ITERATE_ERRORS(REGISTER_ERROR, ANNOUNCE_SECTION) \
        REGISTER_ERROR(NO_ERROR) \
        \
        ANNOUNCE_SECTION("Generic Errors") \
        REGISTER_ERROR(ACCESS_DENIED) \
        REGISTER_ERROR(INVALID_FILE_NAME) \
        REGISTER_ERROR(FILE_NOT_FOUND) \
        REGISTER_ERROR(UNKNOWN_OPEN_ERROR) \
        REGISTER_ERROR(UNKNOWN_READ_ERROR) \
        REGISTER_ERROR(TRY_READ_PAST_EOF) \
        \
        ANNOUNCE_SECTION("C FILE Specific Errors") \
        REGISTER_ERROR(CFILE_SEEK_ERROR) \
        REGISTER_ERROR(CFILE_TELL_ERROR) \
        \
        ANNOUNCE_SECTION("POSIX Specific Errors") \
        REGISTER_ERROR(POSIX_STAT_ERROR)

#define REGISTER_ERROR(error) error,
        IO_ITERATE_ERRORS(REGISTER_ERROR, IO_IGNORE_SECTION_ANNOUNCEMENTS)
#undef REGISTER_ERROR
    };

    struct ErrorDescription {
        [[nodiscard]] inline constexpr
        ErrorDescription(std::string_view section, std::string_view code)
                : m_section(section)
                , m_code(code) {
        }

        [[nodiscard]] inline constexpr std::string_view
        section() const noexcept {
            return m_section;
        }

        [[nodiscard]] inline constexpr std::string_view
        code() const noexcept {
            return m_code;
        }

    private:
        std::string_view m_section;
        std::string_view m_code;
    };

    [[nodiscard]] inline constexpr ErrorDescription
    describeError(Error error) {
        std::string_view section{"Initial Section"};
#define IO_DESCRIBE_ERROR_DEFINE_SECTION(string) section = string;

#define REGISTER_ERROR(error2) \
        if (error == Error::error2) \
            return ErrorDescription{section, #error2};

        IO_ITERATE_ERRORS(REGISTER_ERROR, IO_DESCRIBE_ERROR_DEFINE_SECTION)

#undef REGISTER_ERROR

        return {"Unknown", "Unknown"};
    }

} // namespace io
