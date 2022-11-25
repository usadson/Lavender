/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

//#ifndef NDEBUG
#   define LAVENDER_SHOW_CONSOLE
#   include <cstdio>
//#endif

namespace platform::win32 {

    class ConsoleManager {
    public:
        [[nodiscard]] static ConsoleManager *
        instance() noexcept;

        [[nodiscard]] 
        ConsoleManager() noexcept;

        ~ConsoleManager() noexcept;

        [[nodiscard]] void *
        findWindowHandle() const noexcept;

        bool 
        initialize() noexcept;

        void 
        pause() noexcept;

        inline constexpr void 
        setKeepOpen(bool keepOpen) noexcept {
            m_keepOpen = keepOpen;
        }

    private:
        bool m_keepOpen{false};

#ifdef LAVENDER_SHOW_CONSOLE
        std::FILE *m_stdin{nullptr};
        std::FILE *m_stdout{nullptr};
        std::FILE *m_stderr{nullptr};
#endif // LAVENDER_SHOW_CONSOLE
    };

} // namespace platform::win32
