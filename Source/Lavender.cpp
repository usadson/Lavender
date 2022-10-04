/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#include "Lavender.hpp"

#include <chrono>
#include <random>

#include "Source/ECS/PointLight.hpp"
#include "Source/Input/Bluetooth/Win32BluetoothManager.hpp"
#include "Source/OpenGL/GLCore.hpp"
#include "Source/Vulkan/VulkanCore.hpp"
#include "Source/Window/GLFWCore.hpp"

#ifdef LAVENDER_WIN32_SUPPORT_ENABLED
#   include "Source/Window/Win32Core.hpp"
#endif

void
Lavender::update(float deltaTime) noexcept {
    static float rot = 0;
    rot += deltaTime;

//    m_mainEntity->transformation().rotation = {rot * 10, 0.0f, 0.0f};
//    m_mainEntity->transformation().rotation = {0.0f, rot * 10, 0.0f};
//    m_mainEntity->transformation().rotation = {0.0f, 0.0f, rot * 10};

    for (auto &entity : std::data(m_scene.entityList())) {
        entity->onUpdate(deltaTime);
    }

    m_controller.rotateYaw = 0;
}

void
Lavender::render() noexcept {
    m_graphicsAPI->renderEntities();
}

void
Lavender::renderFirstFrameAsLavenderIsLoading() noexcept {
    m_windowAPI->preLoop();

    if (m_windowAPI->shouldClose())
        return;

    render();
    m_windowAPI->postLoop();
}

base::ExitStatus 
Lavender::run() {
    auto previousFrameTime = std::chrono::steady_clock::now();

    m_camera = static_cast<interface::FreeCamera *>(
        m_scene.entityList().add(std::make_unique<interface::FreeCamera>(&m_controller)));

#ifdef LAVENDER_WIN32_SUPPORT_ENABLED
    m_windowAPI = std::make_unique<window::Win32Core>();
    if (!m_windowAPI->initialize(GraphicsAPI::Name::OPENGL)) {
        std::puts("[Lavender] Failed to initialize Win32-based window");
        m_windowAPI = nullptr;
    }
#endif

    if (!m_windowAPI) {
        m_windowAPI = std::make_unique<window::GLFWCore>();

        if (!m_windowAPI->initialize(GraphicsAPI::Name::OPENGL)) {
            return base::ExitStatus::FAILED_INITIALISING_WINDOW_API;
        }
    }

    m_windowAPI->onCloseRequested +=[](window::CloseRequestedEvent &event) -> base::Error {
        std::printf("[Lavender] Close requested: %s\n", toString(event.reason()).data());
        return base::Error::success();
    };

    m_windowAPI->setDarkMode(DarkModeOption::HONOR_USER_SETTINGS);

    m_windowAPI->setIcon("Resources/lavender.png").displayErrorMessageBox();
    m_windowAPI->setVisibility(WindowVisibilityOption::SHOW);

    m_windowAPI->requestVSyncMode(false);
    setupController();

    auto glVersion = m_windowAPI->queryGLContextVersion();
    std::printf("[Lavender] Context Version: %i.%i.%i\n", glVersion.major, glVersion.minor, glVersion.revision);

    m_graphicsAPI = std::make_unique<gle::Core>(&m_scene, &m_controller, m_camera);
//    m_graphicsAPI = std::make_unique<vke::Core>(&m_scene, &m_controller, m_camera);
    if (!m_graphicsAPI->initialize(m_windowAPI.get())) {
        return base::ExitStatus::FAILED_INITIALISING_GRAPHICS_API;
    }

    m_windowAPI->registerGraphicsAPI(m_graphicsAPI.get());
    renderFirstFrameAsLavenderIsLoading();

    {
        auto scene = m_graphicsAPI->loadGLTFScene("Resources/Assets/Models/Scene.gltf");
        if (scene == nullptr) {
            std::puts("[Lavender] Failed to load Scene.gltf!");
            return base::ExitStatus::FAILED_LOADING_MODEL;
        }

        m_scene.import(std::move(*scene));
    }
    renderFirstFrameAsLavenderIsLoading();

    auto *planeTexture = m_graphicsAPI->createTexture(resources::TextureInput{
        "Resources/Assets/Textures/bricks03 diffuse 1k.jpg",
        true
    });

    auto *dispatcherTexture = m_graphicsAPI->createTexture(resources::TextureInput{
        "Resources/Assets/Textures/Dispatcher_Orange.png",
        true
    });

    assert(planeTexture != nullptr);
    assert(dispatcherTexture != nullptr);

    for (const auto &entity : m_scene.entityList().data()) {
        if (entity->modelDescriptor() == nullptr)
            continue;

        auto *texture = planeTexture;

        if (entity->name() == "Dispatcher") {
            m_mainEntity = entity.get();
            texture = dispatcherTexture;
        } else if (entity->name() == "Plane") {
            entity->transformation().scaling = {10.0f, 10.0f, 10.0f};
            entity->transformation().translation = {0.0f, -2.0f, 0.0f};
        }

        const_cast<resources::ModelDescriptor *>(const_cast<ecs::Entity *>(entity.get())->modelDescriptor())
            ->attachTexture(resources::TextureSlot::ALBEDO, texture);
    }
    
    renderFirstFrameAsLavenderIsLoading();

    if (m_mainEntity == nullptr) {
        std::puts("[GL] Failed to find main entity 'Dispatcher'");
        return base::ExitStatus::FAILED_LOADING_MODEL;
    }

//    auto *planeTexture = m_graphicsAPI->createTexture(resources::TextureInput{
//        "Resources/Assets/Textures/bricks03 diffuse 1k.jpg",
//        true
//    });
//
//    auto *planeGeometry = m_graphicsAPI->loadGLTFModelGeometry("Resources/Assets/Models/Plane.gltf");
//    assert(planeGeometry != nullptr);
//    auto planeModelDescriptor = resources::ModelDescriptor{planeGeometry};
//    planeModelDescriptor.attachTexture(resources::TextureSlot::ALBEDO, planeTexture);
//    auto *planeModel = m_graphicsAPI->uploadModelDescriptor(std::forward<resources::ModelDescriptor>(planeModelDescriptor));
//    assert(planeModel != nullptr);
//    auto *planeEntity = m_scene.entityList().create(planeModel);
//    assert(planeEntity != nullptr);
//    planeEntity->transformation().scaling = {10.0f, 10.0f, 10.0f};

    setupLights();
    renderFirstFrameAsLavenderIsLoading();

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
        if (m_windowAPI->shouldClose())
            break;

        render();
        m_windowAPI->postLoop();
    }

    return base::ExitStatus::SUCCESS;
}

void
Lavender::setupController() noexcept {
#if 0
    m_bluetoothManager = std::make_unique<input::bluetooth::Win32BluetoothManager>();
    if (auto error = m_bluetoothManager->initialize())
        error.displayErrorMessageBox();
#endif


    m_windowAPI->registerKeyboardCallback([&](input::KeyboardUpdate update) {
        switch (update.key) {
#define LAVENDER_MAP_KEY(key, entry) \
        case input::KeyboardKey::key: \
            m_controller.entry = update.action != input::KeyboardAction::RELEASE; \
            break;
            LAVENDER_MAP_KEY(W, moveForward)
            LAVENDER_MAP_KEY(S, moveBackward)
            LAVENDER_MAP_KEY(A, moveLeft)
            LAVENDER_MAP_KEY(D, moveRight)
            LAVENDER_MAP_KEY(SPACE, moveUp)
            LAVENDER_MAP_KEY(LEFT_SHIFT, moveDown)
        case input::KeyboardKey::NUMPAD9: {
            auto pos = m_camera->position();
            std::printf("[Lavender] Debug Position: %f %f %f\n", static_cast<double>(pos.x()),
                static_cast<double>(pos.y()), static_cast<double>(pos.z()));
            break;
        }
        case input::KeyboardKey::ESCAPE:
            if (auto error = m_windowAPI->requestClose(window::CloseRequestedEvent::Reason::APPLICATION))
                error.displayErrorMessageBox();
            break;
        default: break;
        }

#ifdef LAVENDER_BUILD_DEBUG
        m_graphicsAPI->onDebugKey(update);
#endif
    });
    m_windowAPI->registerMouseCallback([&](input::MouseUpdate update) {
        if (m_windowAPI->mouseGrabbed() == input::MouseGrabbed::NO) {
            if (update.button == input::MouseButton::LEFT && update.isPressed)
                m_windowAPI->setMouseGrabbed(input::MouseGrabbed::YES);
            return;
        }

        if (update.button == input::MouseButton::RIGHT && update.isPressed) {
            m_windowAPI->setMouseGrabbed(input::MouseGrabbed::NO);
            return;
        }

        if (update.button != input::MouseButton::NONE)
            return;

        m_controller.rotatePitch -= update.moveX;
        m_controller.rotateYaw -= update.moveY;
    });
}

void
Lavender::setupLights() noexcept {
//    constexpr const std::size_t lightCount = 32;
//    std::default_random_engine engine;
//    std::uniform_real_distribution<float> positionDistrib(-3.0f, 3.0f);
//    std::uniform_real_distribution<float> colorDistrib(0.5, 1.0);
//
//    for (std::size_t i = 0; i < lightCount; i++) {
//        auto pointLight = std::make_unique<ecs::PointLight>(
//            math::Vector3f{positionDistrib(engine), positionDistrib(engine), positionDistrib(engine)},
//            math::Vector3f{colorDistrib(engine), colorDistrib(engine), colorDistrib(engine)},
//            3.0f,
//            0.2f,
//            0.0f,
//            0.0f,
//            1.0f
//        );
//
//        m_scene.entityList().add(std::move(pointLight));
//    }

    m_scene.entityList().add(std::make_unique<ecs::PointLight>(
            math::Vector3f{-0.570345f, 0.343115f, -0.175136f},
            math::Vector3f{1.0f, 1.0f, 1.0f},
            3.0f,
            0.2f,
            0.0f,
            0.0f,
            1.0f
    ));
}
