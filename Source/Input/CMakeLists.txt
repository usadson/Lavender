# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
# All Rights Reserved.

option(LAVENDER_ENABLE_BLUETOOTH "Enable Bluetooth support (for e.g. game controllers)" ON)

add_library(InputLibrary INTERFACE)

if (LAVENDER_ENABLE_BLUETOOTH)
	add_library(BluetoothLibrary Bluetooth/Win32BluetoothManager.cpp)
	target_compile_definitions(BluetoothLibrary PUBLIC LAVENDER_ENABLE_BLUETOOTH)
	target_link_libraries(BluetoothLibrary PRIVATE Bthprops.lib project_diagnostics)
	#if (ENABLE_WIN32)
		target_compile_definitions(BluetoothLibrary PUBLIC LAVENDER_WIN32_SUPPORT_ENABLED)
	#endif()

	target_link_libraries(InputLibrary INTERFACE BluetoothLibrary)
endif()