/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <cstdint>

namespace base {

	struct Version {
        std::uint8_t major;
        std::uint8_t minor;
        std::uint8_t patch;

        [[nodiscard]] inline constexpr std::uint32_t
        asCombined() const noexcept {
            return (static_cast<std::uint32_t>(major) << 16)
                | (static_cast<std::uint32_t>(minor) << 8)
                | (static_cast<std::uint32_t>(patch));
        }
	};

} // namespace base
