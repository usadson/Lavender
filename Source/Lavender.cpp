﻿/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Lavender.hpp"

#include <chrono>

#include "Source/OpenGL/GLCore.hpp"
#include "Source/Vulkan/VulkanCore.hpp"
#include "Source/Window/GLFWCore.hpp"

void
Lavender::update(float deltaTime) noexcept {
    static float rot = 0;
    rot += deltaTime;

//    m_mainEntity->transformation().rotation = {rot * 10, 0.0f, 0.0f};
    m_mainEntity->transformation().rotation = {rot * 10, rot * 10, 0.0f};
//    m_mainEntity->transformation().rotation = {0.0f, 0.0f, rot * 10};
}

void
Lavender::render() noexcept {
    m_graphicsAPI->renderEntities();
}

base::ExitStatus
Lavender::run() {
    auto previousFrameTime = std::chrono::steady_clock::now();

    m_camera = static_cast<interface::FreeCamera *>(m_entityList.add(std::make_unique<interface::FreeCamera>(&m_controller)));

    m_windowAPI = std::make_unique<window::GLFWCore>();
    if (!m_windowAPI->initialize(GraphicsAPI::Name::OPENGL)) {
        return base::ExitStatus::FAILED_INITIALISING_WINDOW_API;
    }

    m_windowAPI->requestVSyncMode(false);

    auto glVersion = m_windowAPI->queryGLContextVersion();
    std::printf("[Lavender] Context Version: %i.%i.%i\n", glVersion.major, glVersion.minor, glVersion.revision);

    m_graphicsAPI = std::make_unique<gle::Core>(&m_entityList);
    if (!m_graphicsAPI->initialize(m_windowAPI.get())) {
        return base::ExitStatus::FAILED_INITIALISING_GRAPHICS_API;
    }

    m_windowAPI->registerGraphicsAPI(m_graphicsAPI.get());

    auto *geometry = m_graphicsAPI->loadGLTFModelGeometry("Resources/Assets/Models/Cube.gltf");
    if (geometry == nullptr) {
        std::printf("[Lavender] Failed to load Cube.gltf model!\n");
        return base::ExitStatus::FAILED_LOADING_MODEL;
    }

    auto *texture = m_graphicsAPI->createTexture(resources::TextureInput{
        "Resources/Assets/Textures/bricks03 diffuse 1k.jpg",
        true
    });

    resources::ModelDescriptor modelInput{geometry};
    modelInput.attachTexture(resources::TextureSlot::ALBEDO, texture);

    auto *model = m_graphicsAPI->uploadModelDescriptor(std::move(modelInput));

    m_mainEntity = m_entityList.create(model);
    m_mainEntity->transformation().translation = {0.0f, 0.0f, 2.0f};
    m_mainEntity->transformation().scaling = {0.5f, 0.5f, 0.5f};

    float temp = 0;
    std::uint16_t frameCount{0};
    while (!m_windowAPI->shouldClose()) {
        ++frameCount;

        const auto thisFrameTime = std::chrono::steady_clock::now();
        const auto deltaTime = duration_cast<std::chrono::duration<float>>(thisFrameTime - previousFrameTime).count();
        previousFrameTime = thisFrameTime;

        temp += deltaTime;
        if (temp >= 1.0f) {
            temp -= 1;
            std::printf("[Lavender] FPS: %hu (deltaTime=%f)\n", frameCount, static_cast<double>(deltaTime));
            frameCount = 0;
        }

        update(deltaTime);

        m_windowAPI->preLoop();
        render();
        m_windowAPI->postLoop();
    }

    return base::ExitStatus::SUCCESS;
}
