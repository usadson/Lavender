/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Source/GraphicsAPI.hpp"

#include "Source/ECS/Scene.hpp"

std::unique_ptr<ecs::Scene>
GraphicsAPI::loadGLTFScene(std::string_view fileName) noexcept  {
    static_cast<void>(fileName);
    return nullptr;
}

#ifdef LAVENDER_BUILD_DEBUG
void
GraphicsAPI::onDebugKey(input::KeyboardUpdate) noexcept {
    // To be overridden
}
#endif
