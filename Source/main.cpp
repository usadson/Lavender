/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <cstdio>
#include <cstdlib>

#include "Lavender.hpp"

int main() {
	Lavender lavender;

	const auto status = lavender.run();
	if (status == base::ExitStatus::SUCCESS) {
		return EXIT_SUCCESS;
	}

	std::printf("Lavender: ExitStatus is %s (%zu)\n",
		base::translateExitStatusToStringView(status).data(),
		static_cast<std::size_t>(status));

	return EXIT_FAILURE;
}
