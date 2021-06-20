/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <string_view>

namespace base {
	
	enum class ExitStatus {

		SUCCESS,

		FAILED_CHOOSING_GRAPHICS_API,
		FAILED_INITIALISING_GRAPHICS_API,

		FAILED_INITIALISING_WINDOW_API,

	};

	[[nodiscard]] inline constexpr std::string_view
	translateExitStatusToStringView(ExitStatus status) noexcept {
		switch (status) {
			case ExitStatus::SUCCESS: return "success";
			case ExitStatus::FAILED_CHOOSING_GRAPHICS_API: return "failed-choosing-graphics-api";
			case ExitStatus::FAILED_INITIALISING_GRAPHICS_API: return "failed-initialising-graphics-api";
			case ExitStatus::FAILED_INITIALISING_WINDOW_API: return "failed-initialising-window-api";
			default: return "(invalid)";
		}
	}

} // namespace Base
