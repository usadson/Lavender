/*
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

//	m_graphicsAPI = std::make_unique<vke::Core>();
        m_graphicsAPI = std::make_unique<gle::Core>();
	if (!m_graphicsAPI->initialize(m_windowAPI.get())) {
		return base::ExitStatus::FAILED_INITIALISING_GRAPHICS_API;
	}

	while (!m_windowAPI->shouldClose()) {
        m_windowAPI->preLoop();

		// do stuff

		m_windowAPI->postLoop();
	}

	return base::ExitStatus::SUCCESS;
}
