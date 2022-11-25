# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_library(IOLibrary OBJECT
		FileInput.cpp
		Format/GLTF/ComponentType.cpp
		Format/GLTF/Context.cpp
		Format/GLTF/ImageLoader.cpp
		Format/Image/BulkImageLoader.cpp
		Format/Image/STBImage.cpp 
)

target_link_libraries(IOLibrary PRIVATE
		nlohmann_json::nlohmann_json 
		fmt::fmt
		project_diagnostics
)
