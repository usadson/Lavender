/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Source/GraphicsAPI.hpp"

#include "Source/ECS/Scene.hpp"

base::ErrorOr<std::unique_ptr<ecs::Scene>>
GraphicsAPI::loadGLTFScene([[maybe_unused]] std::string_view fileName) noexcept  {
    return {nullptr};
}

#ifdef LAVENDER_BUILD_DEBUG
void
GraphicsAPI::onDebugKey(input::KeyboardUpdate) noexcept {
    // To be overridden
}

#endif

base::ErrorOr<resources::ModelGeometryDescriptor*>
GraphicsAPI::createSphere([[maybe_unused]] std::size_t stacks, [[maybe_unused]] std::size_t sectors,
    [[maybe_unused]] float size) noexcept {
    return { nullptr };
}
