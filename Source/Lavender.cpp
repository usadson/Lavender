﻿/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Lavender.hpp"

#include "Source/OpenGL/GLCore.hpp"
#include "Source/Vulkan/VulkanCore.hpp"
#include "Source/Window/GLFWCore.hpp"

base::ExitStatus
Lavender::run() {
    m_windowAPI = std::make_unique<window::GLFWCore>();
    if (!m_windowAPI->initialize(GraphicsAPI::Name::OPENGL)) {
        return base::ExitStatus::FAILED_INITIALISING_WINDOW_API;
    }

    auto glVersion = m_windowAPI->queryGLContextVersion();
    std::printf("[Lavender] Context Version: %i.%i.%i\n", glVersion.major, glVersion.minor, glVersion.revision);

//	m_graphicsAPI = std::make_unique<vke::Core>();
    m_graphicsAPI = std::make_unique<gle::Core>();
    if (!m_graphicsAPI->initialize(m_windowAPI.get())) {
        return base::ExitStatus::FAILED_INITIALISING_GRAPHICS_API;
    }

    static_cast<void>(
        m_graphicsAPI->createModel(resources::ModelGeometry{
            .vertices {
                 0.0f,  0.5f,
                 0.5f, -0.5f,
                -0.5f, -0.5f
            },
            .indices {}
        })
    );

    while (!m_windowAPI->shouldClose()) {
        m_windowAPI->preLoop();

        m_graphicsAPI->renderEntities();

        m_windowAPI->postLoop();
    }

    return base::ExitStatus::SUCCESS;
}
