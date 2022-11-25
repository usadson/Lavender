# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
#
# Copyright (C) 2021 - 2022 Tristan Gerritsen <tristan@thewoosh.org>
# All Rights Reserved.

add_library(NetworkCore OBJECT
		"UDP/Socket.cpp"
)

target_compile_definitions(NetworkCore PRIVATE LAVENDER_WIN32_SUPPORT_ENABLED)

target_link_libraries(NetworkCore
	PRIVATE project_diagnostics
)
