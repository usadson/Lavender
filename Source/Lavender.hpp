/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <memory>

#include "Source/Base/ExitStatus.hpp"
#include "Source/Base/Library.hpp"
#include "Source/ECS/Scene.hpp"
#include "Source/Input/Controller.hpp"
#include "Source/Interface/FreeCamera.hpp"
#include "Source/Window/WindowAPI.hpp"
#include "Source/GraphicsAPI.hpp"

#ifdef LAVENDER_ENABLE_BLUETOOTH
#   include "Source/Input/Bluetooth/BluetoothManager.hpp"
#endif // LAVENDER_ENABLE_BLUETOOTH

class Lavender {
    std::unique_ptr<GraphicsAPI> m_graphicsAPI{nullptr};
    std::unique_ptr<WindowAPI> m_windowAPI{ nullptr };

#ifdef LAVENDER_ENABLE_BLUETOOTH
    std::unique_ptr<input::bluetooth::BluetoothManager> m_bluetoothManager{};
#endif // LAVENDER_ENABLE_BLUETOOTH

    input::Controller m_controller{};
    interface::FreeCamera *m_camera{};

    ecs::Scene m_scene{ecs::EntityList{}};
    ecs::Entity *m_mainEntity{nullptr};

    void
    render() noexcept;

    void 
    renderFirstFrameAsLavenderIsLoading() noexcept;

    void
    setupController() noexcept;

    void
    setupLights() noexcept;

    void
    update(float deltaTime) noexcept;

public:
    [[nodiscard]] base::ExitStatus
    run();
};
