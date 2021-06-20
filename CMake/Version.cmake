# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

add_compile_definitions(LAVENDER_VERSION_MAJOR=${VERSION_MAJOR}
						LAVENDER_VERSION_MINOR=${VERSION_MINOR}
						LAVENDER_VERSION_PATCH=${VERSION_PATCH})
