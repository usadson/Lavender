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

#include "Source/Base/About.hpp"
#include "Source/ECS/PointLight.hpp"
#include "Source/Input/Bluetooth/Win32BluetoothManager.hpp"
#include "Source/OpenGL/GLCore.hpp"
#include "Source/Resources/FileResourceLocation.hpp"
#include "Source/Resources/MaterialDescriptor.hpp"
#include "Source/Vulkan/VulkanCore.hpp"
#include "Source/Window/GLFWCore.hpp"

#include <iostream>
#include <ranges>

#define FMT_HEADER_ONLY
#include <fmt/format.h>

#ifdef LAVENDER_WIN32_SUPPORT_ENABLED
#   include "Source/Window/Win32Core.hpp"
#endif

base::Error
Lavender::processEvents() noexcept {
    return mainThreadQueue.processAll();
}

void
Lavender::update(double deltaTime) noexcept {
    m_scene.fireUpdate(static_cast<float>(deltaTime));
    m_controller.rotateYaw = 0;
}

base::Error
Lavender::refreshTitle(const std::string &graphicsModeName) noexcept {
    std::stringstream stream;
    stream << base::About::applicationName << " v" << static_cast<std::size_t>(base::About::applicationVersion.major()) << '.'
           << static_cast<std::size_t>(base::About::applicationVersion.minor());

    if (base::About::applicationVersion.patch() != 0)
        stream << '.' << static_cast<std::size_t>(base::About::applicationVersion.patch());

    stream << " (" << base::About::engineName << " v" << static_cast<std::size_t>(base::About::engineVersion.major()) << '.'
           << static_cast<std::size_t>(base::About::engineVersion.minor());

    if (base::About::engineVersion.patch() != 0)
        stream << '.' << static_cast<std::size_t>(base::About::engineVersion.patch());

    stream << ')';
    if (!graphicsModeName.empty())
        stream << " " << graphicsModeName;

    return m_windowAPI->setTitle(stream.str());
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

base::Error
Lavender::run() {
    base::FunctionErrorGenerator errors{"LavenderCore", "Lavender"};
    auto previousFrameTime = std::chrono::steady_clock::now();

    m_camera = static_cast<interface::FreeCamera *>(
        m_scene.entityList().add(std::make_unique<interface::FreeCamera>(&m_controller)));

#ifdef LAVENDER_WIN32_SUPPORT_ENABLED
    m_windowAPI = std::make_unique<window::Win32Core>();
    if (auto error = m_windowAPI->initialize(GraphicsAPI::Name::OPENGL)) {
        std::puts("[Lavender] Failed to initialize Win32-based window");
        error.displayErrorMessageBox();
        m_windowAPI = nullptr;
    }
#endif

    if (!m_windowAPI) {
        m_windowAPI = std::make_unique<window::GLFWCore>();

        if (auto error = m_windowAPI->initialize(GraphicsAPI::Name::OPENGL)) {
            return error;
        }
    }

    if (auto error = m_windowAPI->enableDragAndDrop(WindowDragAndDropOption::ENABLED))
        return error;

    m_windowAPI->onCloseRequested +=[](window::CloseRequestedEvent &event) -> base::Error {
        std::printf("[Lavender] Close requested: %s\n", toString(event.reason()).data());
        return base::Error::success();
    };

    m_windowAPI->onDragEnter += [&](input::DragEnterEvent &event) -> base::Error {
        event.allowDrop();
        std::size_t index = 0;
        while (index < m_scene.entityList().data().size()) {
            auto &entity = *m_scene.entityList().data()[index];
            if (entity.name() == "Camera")
                ++index;
            else {
                fmt::print("Deleting entity \"{}\" @ {:p}\n", entity.name(), static_cast<const void *>(&entity));
                m_scene.entityList().remove(&entity);
            }
        }
        return base::Error::success();
    };

    m_windowAPI->onDragOver += [](input::DragOverEvent &event) -> base::Error {
        event.allowDrop();
        return base::Error::success();
    };

    m_windowAPI->onDrop += [&](input::DropEvent &event) -> base::Error {
        event.allowDrop();

        assert(event.data().type() == input::DragObject::Type::FILE_LOCATIONS);
        const auto &locations = event.data().asFileLocations();
        assert(!locations.empty());

        TRY_GET_VARIABLE(scene, m_graphicsAPI->loadGLTFScene(locations.front()))
        m_scene.import(std::move(*scene));

        return base::Error::success();
    };

    m_windowAPI->setDarkMode(DarkModeOption::HONOR_USER_SETTINGS);
    
    TRY(m_windowAPI->setIcon("Resources/lavender.png"));
    m_windowAPI->setVisibility(WindowVisibilityOption::SHOW);

    m_windowAPI->requestVSyncMode(false);
    setupController();

    auto glVersion = m_windowAPI->queryGLContextVersion();
    std::printf("[Lavender] Context Version: %i.%i.%i\n", glVersion.major, glVersion.minor, glVersion.revision);

    m_graphicsAPI = std::make_unique<gle::Core>(&m_scene, &m_controller, m_camera);
    TRY(m_graphicsAPI->initialize(m_windowAPI.get()))
    
    m_graphicsAPI->onLocateResource += [&](resources::ResourceLocateEvent &event) -> base::Error {
        //std::cout << "[ResourceLocateEvent] ResourceName: " << event.resourceName() << '\n';

        auto checkDirectory = [&](const std::filesystem::path &path) -> bool {
            //std::cout << "[ResourceLocateEvent] Looking in: " << path << '\n';

            switch (event.resourceType()) {
            case resources::ResourceLocateEvent::ResourceType::IMAGE:
            case resources::ResourceLocateEvent::ResourceType::BINARY:
                for (const auto &entry : std::filesystem::directory_iterator(path)) {
                    //std::cout << "  > " << entry.path().filename() << '\n';
                    if (!entry.is_regular_file())
                        continue;

                    if (entry.path().stem() != event.resourceName() && entry.path().filename() != event.resourceName())
                        continue;

                    //std::cout << "[ResourceLocateEvent] Found the resource: " << entry << '\n';

                    event.location()
                        = std::make_unique<resources::FileResourceLocation>(std::filesystem::path(entry.path()));
                    return true;
                }

                break;
            default:
                return false;
            }

            return false;
        };
        
        for (auto &it : std::ranges::reverse_view(event.directoryHints())) {
            std::filesystem::path path(it);

            if (checkDirectory(path))
                return base::Error::success();

            path.append("Resources/Assets/");
            if (!std::filesystem::is_directory(path))
                continue;

            if (checkDirectory(path))
                return base::Error::success();

            path.append("Textures");
            if (!std::filesystem::is_directory(path))
                continue;

            if (checkDirectory(path))
                return base::Error::success();
        }

        std::stringstream stream;
        stream << "Couldn't locate resource!\n\nResource Name: \"" << event.resourceName() << "\"\nLooking in "
               << event.directoryHints().size();
        if (event.directoryHints().size() == 1)
            stream << "directory.\n";
        else
            stream << "directories.\n";

        for (const auto &directory : event.directoryHints())
            stream << "Directory : \"" << directory << "\"\n";

        static std::string errorDescription;
        errorDescription = stream.str();
        return base::Error("LavenderCore", "Lavender", "Resolve ResourceLocateEvent", errorDescription);
    };

#ifndef NDEBUG
    m_graphicsAPI->onGraphicsModeChange += [&](const std::string &modeName) {
        return refreshTitle(modeName);
    };
#endif

    m_windowAPI->registerGraphicsAPI(m_graphicsAPI.get());

    renderFirstFrameAsLavenderIsLoading();

    {
        //auto scene = m_graphicsAPI->loadGLTFScene("Resources/Assets/Models/Scene.gltf");
        //auto scene = m_graphicsAPI->loadGLTFScene("C:\\Data\\MKWii\\DryBones\\output\\modelout.gltf");
        std::size_t requiredDrive{}, requiredPath{};
        std::array<char, 8> homeDrive;
        getenv_s(&requiredDrive, homeDrive.data(), homeDrive.size(), "HOMEDRIVE");
        std::array<char, 128> homePath;
        getenv_s(&requiredPath, homePath.data(), homePath.size(), "HOMEPATH");

        TRY_GET_VARIABLE(scene, m_graphicsAPI->loadGLTFScene(
            fmt::format("{}{}\\3D Objects\\gltfSampleModels\\2.0\\Sponza\\glTF\\Sponza.gltf", homeDrive.data(), homePath.data())
            //fmt::format("{}{}\\3D Objects\\0.gltf", homeDrive.data(), homePath.data())
            //"C:/Data/MKWii/Luigi Circuit/output/course.gltf"
            //"C:/Data/Assets/ThinMatrixSkeletalAnimationDemo/scene.gltf"
            //"C:/Data/ANCH/ICCRMMCHN/scene.gltf"
        ));
        //TRY_GET_VARIABLE(scene, m_graphicsAPI->loadGLTFScene("Resources/Assets/Models/Scene.gltf"));

        m_scene.import(std::move(*scene));
    }
    renderFirstFrameAsLavenderIsLoading();

    for (const auto &entity : m_scene.entityList().data()) {
        m_mainEntity = entity.get();
    }
    
    renderFirstFrameAsLavenderIsLoading();

    if (m_mainEntity == nullptr)
        return errors.error("Initializing entities in scene", "Failed to find main entity 'Dispatcher'");

    setupLights();
    renderFirstFrameAsLavenderIsLoading();

    double temp = 0;
    std::uint16_t frameCount{0};
    while (!m_windowAPI->shouldClose()) {
        ++frameCount;

        const auto thisFrameTime = std::chrono::steady_clock::now();
        const auto deltaTime = duration_cast<std::chrono::duration<double>>(thisFrameTime - previousFrameTime).count();
        previousFrameTime = thisFrameTime;

        temp += deltaTime;
        if (temp >= 1.0f) {
            if (temp >= 2.0f) {
                std::printf("[Lavender] 1 frame after %f seconds\n", temp);
                temp = 0;
            } else {
                temp -= 1;
                std::printf("[Lavender] FPS: %hu (deltaTime=%f)\n", frameCount, deltaTime);
            }
            frameCount = 0;
        }

        if (auto error = processEvents())
            error.displayErrorMessageBox();

        update(deltaTime);

        m_windowAPI->preLoop();
        if (m_windowAPI->shouldClose())
            break;

        render();
        m_windowAPI->postLoop();
    }

    return base::Error::success();
}

void
Lavender::setupController() noexcept {
#if 1
    m_bluetoothManager = std::make_unique<input::bluetooth::Win32BluetoothManager>();
    if (auto error = m_bluetoothManager->initialize())
        error.displayErrorMessageBox();

    if (auto error = m_deviceManager.initialize())
        error.displayErrorMessageBox();
#endif

    m_windowAPI->onKeyPressed += [&](const input::KeyPressedEvent &event) -> base::Error { 
        switch (event.key()) {
#define LAVENDER_MAP_KEY(key, entry) \
            case input::KeyboardKey::key: \
                m_controller.entry = true; \
                break;
            LAVENDER_MAP_KEY(W, moveForward)
            LAVENDER_MAP_KEY(S, moveBackward)
            LAVENDER_MAP_KEY(A, moveLeft)
            LAVENDER_MAP_KEY(D, moveRight)
            LAVENDER_MAP_KEY(SPACE, moveUp)
            LAVENDER_MAP_KEY(LEFT_SHIFT, moveDown)
            case input::KeyboardKey::ESCAPE:
                TRY(m_windowAPI->requestClose(window::CloseRequestedEvent::Reason::APPLICATION))
                break;
            case input::KeyboardKey::NUMPAD9: {
                auto pos = m_camera->position();
                std::printf("[Lavender] Debug Position: %f %f %f\n", static_cast<double>(pos.x()),
                    static_cast<double>(pos.y()), static_cast<double>(pos.z()));
                break;
            }
            default:
                break;
        }

        return base::Error::success();
    };

    m_windowAPI->onKeyReleased += [&](const input::KeyReleasedEvent &event) -> base::Error {
        switch (event.key()) {
#undef LAVENDER_MAP_KEY
#define LAVENDER_MAP_KEY(key, entry) \
            case input::KeyboardKey::key: \
                m_controller.entry = false; \
                break;
            LAVENDER_MAP_KEY(W, moveForward)
            LAVENDER_MAP_KEY(S, moveBackward)
            LAVENDER_MAP_KEY(A, moveLeft)
            LAVENDER_MAP_KEY(D, moveRight)
            LAVENDER_MAP_KEY(SPACE, moveUp)
            LAVENDER_MAP_KEY(LEFT_SHIFT, moveDown)
            default:
                break;
        }

        return base::Error::success();
    };
    
    m_windowAPI->registerMouseCallback([&](const input::MouseUpdate &update) {
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

#if 0
    m_scene.entityList().add(std::make_unique<ecs::PointLight>(
        math::Vector3f{0.0f, 0.0f, 0.0f},
        math::Vector3f{1.0f, 0.0f, 1.0f},
        3.0f,
        0.2f,
        0.0f,
        0.0f,
        1.0f
    ));
#endif
}
