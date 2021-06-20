/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace io {

    enum class Error {
        NO_ERROR,

        //
        // Generic Errors
        //
        ACCESS_DENIED,
        INVALID_FILE_NAME,
        FILE_NOT_FOUND,
        UNKNOWN_OPEN_ERROR,
        UNKNOWN_READ_ERROR,
        TRY_READ_PAST_EOF,

        //
        // C FILE Specific Errors
        //
        CFILE_SEEK_ERROR,
        CFILE_TELL_ERROR,

        //
        // POSIX Specific Errors
        //
        POSIX_STAT_ERROR,

    };

} // namespace io
