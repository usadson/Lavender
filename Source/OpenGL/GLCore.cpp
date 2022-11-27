/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * OpenGL® and the oval logo are trademarks or registered trademarks of
 * Hewlett Packard Enterprise in the United States and/or other
 * countries worldwide.
 */

#include "Source/OpenGL/GLCore.hpp"

#include <type_traits>

#define FMT_HEADER_ONLY
#include <fmt/core.h>

#include <GL/glew.h>

#include "Source/ECS/EntityList.hpp"
#include "Source/Interface/Camera.hpp"
#include "Source/OpenGL/DebugMessenger.hpp"
#include "Source/OpenGL/Renderer/DeferredRenderer.hpp"
#include "Source/Resources/MaterialDescriptor.hpp"
#include "Source/Window/WindowAPI.hpp"

namespace gle {

    static_assert(std::is_same_v<GLfloat, float>);
    constexpr base::FunctionErrorGenerator errors{ "OpenGLCore", "Core" };

    Core::Core(ecs::Scene const *scene,
               input::Controller const *controller,
               interface::Camera const *camera) noexcept
        : GraphicsAPI(scene, controller, camera) {
    }
    
    Core::~Core() noexcept = default;

    base::ErrorOr<resources::MaterialDescriptor *>
    Core::createMaterial() noexcept {
        return m_materialDescriptors.emplace_back(std::make_unique<resources::MaterialDescriptor>()).get();
    }

    base::ErrorOr<resources::ModelGeometryDescriptor *>
    Core::createModelGeometry(const resources::ModelGeometry &geometry) noexcept {
        GLuint vao{};
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        auto ebo = createElementBuffer(geometry.indices);
        if (!ebo.has_value())
            return errors.error("Create Element Buffer", "Unknown Error");

        auto vbo = createVertexBuffer(geometry.vertices);
        if (!vbo.has_value())
            return errors.error("Create Vertex Buffer", "Unknown Error");

        auto tbo = createTextureBuffer(geometry.textureCoordinates);
        if (!tbo.has_value())
            return errors.error("Create Texture Buffer", "Unknown Error");

        glEnableVertexAttribArray(m_renderer->attributeLocations().position);
        glEnableVertexAttribArray(m_renderer->attributeLocations().textureCoordinates);

        glBindVertexArray(0);
        m_geometryDescriptors.push_back(std::make_unique<ModelGeometryDescriptor>(
                static_cast<GLsizei>(geometry.vertices.size()), vao, vbo.value(), ebo.value(), tbo.value(), 0, static_cast<GLsizei>(std::size(geometry.indices)), GL_UNSIGNED_INT));
        return m_geometryDescriptors.back().get();
    }

    std::optional<unsigned int>
    Core::createElementBuffer(const std::vector<resources::ModelGeometry::IndexType> &indices) const noexcept {
        GLuint ebo{};

        glGenBuffers(1, &ebo);
        if (glGetError() != GL_NO_ERROR)
            return std::nullopt;

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        const auto size = static_cast<GLsizeiptr>(std::size(indices) * sizeof(indices[0]));
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, std::data(indices), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return ebo;
    }

    base::ErrorOr<resources::ModelGeometryDescriptor *>
    Core::createSphere(std::size_t stackCount, std::size_t sectorCount, float radius) noexcept {
        auto checkError = [&] {
            if (GLenum err = glGetError(); err != GL_NO_ERROR) {
                while (true) {
                    printf("[GL] Core: Error: %u\n", err);

                    static_cast<void>(std::fflush(stdout));
                    assert(false);

                    if (err == GL_NO_ERROR)
                        break;

                    err = glGetError();
                }

                static_cast<void>(std::fflush(stdout));
                assert(false);
            }
        };

        checkError();

        GLuint vertexArray;
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);

        checkError();

        std::array<GLuint, 4> bufferObjects{};
        glGenBuffers(static_cast<GLsizei>(bufferObjects.size()), bufferObjects.data());
        checkError();

        const auto elementBufferObject = bufferObjects[0];
        const auto vertexBufferObject = bufferObjects[1];
        const auto normalBufferObject = bufferObjects[2];
        const auto textureBufferObject = bufferObjects[3];

        const auto attributes = m_renderer->attributeLocations();

        std::vector<math::Vector3f> vertices;
        std::vector<math::Vector3f> normals;
        std::vector<math::Vector2f> textureCoordinates;

        const auto elementCount = (stackCount + 1) * (sectorCount + 1);
        vertices.reserve(elementCount);
        normals.reserve(elementCount);
        textureCoordinates.reserve(elementCount);
        
        const float lengthInv = 1.0f / radius;

        const float sectorStep = 2 * std::numbers::pi_v<float> / static_cast<float>(sectorCount);
        const float stackStep = std::numbers::pi_v<float> / static_cast<float>(stackCount);

        for (std::size_t i = 0; i <= stackCount; ++i) {
            const auto stackAngle = std::numbers::pi_v<float> / 2 - static_cast<float>(i) * stackStep; // starting from pi/2 to -pi/2
            const auto xy = radius * cosf(stackAngle); // r * cos(u)
            const auto z = radius * sinf(stackAngle); // r * sin(u)

            // add (sectorCount+1) vertices per stack
            // the first and last vertices have same position and normal, but different tex coords
            for (std::size_t j = 0; j <= sectorCount; ++j) {
                const float sectorAngle = sectorStep * static_cast<float>(j);

                // vertex position (x, y, z)
                float x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
                float y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
                vertices.emplace_back(x, y, z);
                
                normals.emplace_back(x * lengthInv, y * lengthInv, z * lengthInv);
                
                textureCoordinates.emplace_back(static_cast<float>(j) / static_cast<float>(sectorCount),
                    static_cast<float>(i) / static_cast<float>(stackCount));
            }
        }

        const auto vertexCount = vertices.size();

        glEnableVertexAttribArray(attributes.position);
        checkError();
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(vertices.size() * sizeof(vertices[0])), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(attributes.position, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        checkError();
        vertices.erase(vertices.begin(), vertices.end());

        glEnableVertexAttribArray(attributes.normal);
        checkError();
        glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(normals.size() * sizeof(normals[0])), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(attributes.normal, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        checkError();
        normals.erase(normals.begin(), normals.end());

        glEnableVertexAttribArray(attributes.textureCoordinates);
        checkError();
        glBindBuffer(GL_ARRAY_BUFFER, textureBufferObject);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(textureCoordinates.size() * sizeof(textureCoordinates[0])), textureCoordinates.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(attributes.textureCoordinates, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        checkError();
        textureCoordinates.erase(textureCoordinates.begin(), textureCoordinates.end());

        std::vector<int> indices;
        //std::vector<int> lineIndices;
        for (std::size_t i = 0; i < stackCount; ++i) {
            auto k1 = static_cast<int>(i * (sectorCount + 1)); // beginning of current stack
            auto k2 = static_cast<int>(k1 + sectorCount + 1); // beginning of next stack

            for (std::size_t j = 0; j < sectorCount; ++j, ++k1, ++k2) {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if (i != 0) {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                // k1+1 => k2 => k2+1
                if (i != (stackCount - 1)) {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }

#if 0
                // store indices for lines
                // vertical lines for all stacks, k1 => k2
                lineIndices.push_back(k1);
                lineIndices.push_back(k2);
                if (i != 0) // horizontal lines except 1st stack, k1 => k+1
                {
                    lineIndices.push_back(k1);
                    lineIndices.push_back(k1 + 1);
                }
#endif
            }
        }

        const auto indexCount = indices.size();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(indices[0])), indices.data(), GL_STATIC_DRAW);
        checkError();
        indices.erase(indices.begin(), indices.end());
        
        m_geometryDescriptors.push_back(std::make_unique<ModelGeometryDescriptor>(static_cast<GLsizei>(vertexCount), vertexArray, vertexBufferObject, elementBufferObject,
            textureBufferObject, normalBufferObject, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT));

        checkError();
        return m_geometryDescriptors.back().get();
    }

    std::optional<unsigned int>
    Core::createTextureBuffer(const std::vector<resources::ModelGeometry::TextureCoordType> &textureCoordinates) const noexcept {
        GLuint tbo{};

        glGenBuffers(1, &tbo);
        if (glGetError() != GL_NO_ERROR)
            return std::nullopt;

        glBindBuffer(GL_ARRAY_BUFFER, tbo);

        const auto size = static_cast<GLsizeiptr>(std::size(textureCoordinates) * sizeof(textureCoordinates[0]));
        glBufferData(GL_ARRAY_BUFFER, size, std::data(textureCoordinates), GL_STATIC_DRAW);

        glVertexAttribPointer(m_renderer->attributeLocations().textureCoordinates, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return tbo;
    }

    std::optional<unsigned int>
    Core::createVertexBuffer(const std::vector<resources::ModelGeometry::VertexType> &vertices) const noexcept {
        GLuint vbo{};

        glGenBuffers(1, &vbo);
        if (glGetError() != GL_NO_ERROR)
            return std::nullopt;

        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        const auto size = static_cast<GLsizeiptr>(std::size(vertices) * sizeof(vertices[0]));
        glBufferData(GL_ARRAY_BUFFER, size, std::data(vertices), GL_STATIC_DRAW);

        glVertexAttribPointer(m_renderer->attributeLocations().position, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return vbo;
    }

    base::Error
    Core::initialize(WindowAPI *window) {
        m_windowAPI = window;
        TRY(initializeGLEW())
        
        const auto contextVersion = m_windowAPI->queryGLContextVersion();

        DebugMessenger::setup();

        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        m_renderer = std::make_unique<DeferredRenderer>(this);

        TRY(m_renderer->setup())

        const auto size = m_windowAPI->queryFramebufferSize();
        onResize({size.x(), size.y()});

        m_windowAPI->onResize += [&](window::ResizeEvent &event) -> base::Error { 
            onResize(event.to());
            return base::Error::success();
        };

#ifdef LAVENDER_BUILD_DEBUG
        m_windowAPI->onKeyPressed += [&](const input::KeyPressedEvent &event) -> base::Error {
            if (event.key() == input::KeyboardKey::NUMPAD0) {
                onGraphicsModeChange.invoke("RenderMode: Default").displayErrorMessageBox();
                m_renderer->renderMode(RenderMode::DEFAULT);
            } else if (event.key() == input::KeyboardKey::NUMPAD1) {
                onGraphicsModeChange.invoke("RenderMode: PositionsBuffer").displayErrorMessageBox();
                m_renderer->renderMode(RenderMode::DEBUG_POSITION);
            } else if (event.key() == input::KeyboardKey::NUMPAD2) {
                onGraphicsModeChange.invoke("RenderMode: SurfaceNormalBuffer").displayErrorMessageBox();
                m_renderer->renderMode(RenderMode::DEBUG_NORMALS);
            } else if (event.key() == input::KeyboardKey::NUMPAD3) {
                onGraphicsModeChange.invoke("RenderMode: DiffuseColorBuffer").displayErrorMessageBox();
                m_renderer->renderMode(RenderMode::DEBUG_COLOR);
            }

            return base::Error::success();
        };
#endif // LAVENDER_BUILD_DEBUG

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glFrontFace(GL_CW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);

        if (contextVersion.major >= 3) {
            glEnable(GL_FRAMEBUFFER_SRGB);
        }

        if (const auto error = glGetError(); error != GL_NO_ERROR)
            return errors.error("Perform finalization steps", fmt::format("GL Error: {}", error));
    
#ifndef NDEBUG
        TRY(onGraphicsModeChange.invoke("RenderMode: Default"))
#endif

        return base::Error::success();
    }

    base::Error
    Core::initializeGLEW() noexcept {
        const auto status = glewInit();

        if (status == GLEW_OK)
            return base::Error::success();

        std::string_view description;
        switch (status) {
            case GLEW_ERROR_NO_GL_VERSION:
                // TODO what does this mean? what is the fix?
                description = "No OpenGL version";
                break;
            case GLEW_ERROR_GL_VERSION_10_ONLY:
                description = "Only OpenGL 1.0 is supported, but 1.1 or above is required.";
                break;
            case GLEW_ERROR_GLX_VERSION_11_ONLY:
                description = "Only GLX 1.1 is supported, but 1.2 or above is required.";
                break;
            case GLEW_ERROR_NO_GLX_DISPLAY:
                description = "No GLX Display found. Are you running headless mode?";
                break;
            default:
                description = reinterpret_cast<const char *>(glewGetErrorString(status));
                break;
        }

        return errors.error("Initialize GLEW", description);
    }

    void
    Core::onResize(math::Size2D<std::uint32_t> size) noexcept {
        glViewport(0, 0, static_cast<GLsizei>(size.width()), static_cast<GLsizei>(size.height()));
        m_renderer->onResize(size);
    }

    void
    Core::renderEntities() noexcept {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_renderer->render();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            printf("[GL] Core: Error occurred whilst rendering entities: %u\n", err);
            assert(false);
        }
    }

    base::ErrorOr<const resources::ModelDescriptor *>
    Core::uploadModelDescriptor(resources::ModelDescriptor &&modelDescriptor) noexcept {
        m_modelDescriptors.push_back(std::make_unique<resources::ModelDescriptor>(std::forward<resources::ModelDescriptor>(modelDescriptor)));
        return m_modelDescriptors.back().get();
    }

} // namespace gle
