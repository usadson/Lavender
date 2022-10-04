/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#include "UWP.hpp"

#ifdef LAVENDER_PLATFORM_UWP_AVAILABLE

#include <winrt/base.h>

namespace winrt::impl {

    template <typename Async> auto 
    wait_for(const Async &async, const Windows::Foundation::TimeSpan &timeout);

} // namespace winrt::impl

#include <winrt/Windows.UI.ViewManagement.h>


using namespace winrt;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::ViewManagement;

namespace platform::win32::uwp {

    [[nodiscard]] inline constexpr bool 
    isColorLight(Color &clr) {
        return (((5 * clr.G) + (2 * clr.R) + clr.B) > (8 * 128));
    }

    DarkMode 
    getDarkModeSetting() noexcept {
        auto settings = UISettings();
        auto background = settings.GetColorValue(UIColorType::Background);

        if (isColorLight(background))
            return DarkMode::OFF;

        return DarkMode::ON;
    }

} // namespace platform::win32::uwp

#endif // LAVENDER_PLATFORM_UWP_AVAILABLE
