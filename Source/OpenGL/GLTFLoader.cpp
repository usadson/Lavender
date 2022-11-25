/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2021 - 2022 Tristan Gerritsen <trisan@thewoosh.org>
 * All Rights Reserved.
 */

#undef ENABLE_VULKAN
#define IO_FORMAT_GLTF_IMPLEMENTATION
#define FMT_HEADER_ONLY

//#define GLTF_NO_TEXTURES
//#define GLTF_DEBUG_WAITING_FOR_TEXTURE_LOADING
//#define GLTF_ANNOUNCE_PARSING

#include "GLCore.hpp"
#include "ECS/PointLight.hpp"
#include "Resources/MemoryResourceLocation.hpp"

#include <fstream>
#include <optional>

#include <cassert>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "Source/Base/ArrayView.hpp"
#include "Source/ECS/Scene.hpp"
#include "Source/IO/Format/GLTF/ComponentType.hpp"
#include "Source/IO/Format/GLTF/Context.hpp"
#include "Source/IO/Format/GLTF/ImageLoader.hpp"
#include "Source/IO/Format/Image/BulkImageLoader.hpp"
#include "Source/Resources/FileResourceLocation.hpp"
#include "ThirdParty/base64.hpp"

#include "Window/WindowAPI.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>

struct Hexdump {
    const unsigned char *m_data;
    const std::size_t m_length;

    [[nodiscard]] inline
    Hexdump(const void *data, std::size_t length)
        : m_data(reinterpret_cast<const unsigned char *>(data))
        , m_length(length) {
    }
};

std::ostream &
operator<<(std::ostream &out, const Hexdump &data) {
    constexpr std::size_t RowSize = 16;

    out.fill('0');
    for (std::size_t i = 0; i < data.m_length; i += RowSize) {
        out << "0x" << std::setw(6) << std::hex << i << ": ";
        for (std::size_t j = 0; j < RowSize; ++j) {
            if (i + j < data.m_length) {
                out << std::hex << std::setw(2) << static_cast<int>(data.m_data[i + j]) << " ";
            } else {
                out << "   ";
            }
        }

        out << " ";
        for (std::size_t j = 0; j < RowSize; ++j) {
            if (i + j < data.m_length) {
                if (std::isprint(data.m_data[i + j])) {
                    out << static_cast<char>(data.m_data[i + j]);
                } else {
                    out << ".";
                }
            }
        }
        out << std::endl;
    }
    return out;
}

namespace gle {

    using namespace io::format::gltf;

    constexpr const unsigned short kComponentTypeSignedByte = 5120;
    constexpr const unsigned short kComponentTypeUnsignedByte = 5121;
    constexpr const unsigned short kComponentTypeSignedShort = 5122;
    constexpr const unsigned short kComponentTypeUnsignedShort = 5123;

    constexpr const unsigned short kComponentTypeUnsignedInt = 5125;
    constexpr const unsigned short kComponentTypeFloat = 5126;

    [[nodiscard]] constexpr std::size_t
    sizeofGLType(GLenum type) noexcept {
        switch (type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            return 1;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            return 2;
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_FLOAT:
            return 4;
        case GL_DOUBLE:
            return 8;
        default:
            assert(false);
            return 0;
        }
    }

    struct GLTFInformation {
        std::string_view name;

        const nlohmann::json &json;
        const std::vector<std::string> &buffers;

        resources::ModelDescriptor *sphereModel;

        Context &context;
        ImageLoader &imageLoader;

        GLuint ebo{};
        GLenum eboType{};
        std::size_t indexCount{};

        base::ArrayView<const math::Vector3f> vertices{};
        base::ArrayView<const math::Vector2f> textureData{};

        std::vector<math::Vector3f> generatedNormals;
        base::ArrayView<const math::Vector3f> normals{};

        GLuint vbo{};
        GLuint tbo{};
        GLuint nbo{};

        GLuint jointVBO{};
        GLuint weightVBO{};

        bool hasSkin{false};
        const nlohmann::json *lightExtensionLights{nullptr};
    };

    struct GLTFResourceInfo {
        const nlohmann::json &accessor;
        const std::string &buffer;
        const nlohmann::json &bufferView;

        GLsizeiptr byteOffset;
        GLsizeiptr byteLength;
        std::optional<GLsizei> byteStride;

        std::string_view bufferPart;
    };

    /**
     * Return views of 0 are error conditions, since no buffers of length 0 are
     * allowed anyway.
     */
    [[nodiscard]] static base::ErrorOr<std::string>
    gltfLoadBuffer(std::string_view uriData, Core *core, std::string_view modelFileName) noexcept {
        base::FunctionErrorGenerator errors{"OpenGLCore", "GLTFLoader"};

        if (uriData.empty())
            return errors.error("Load buffer", "URI data is empty");

        if (!uriData.starts_with("data:")) {
            resources::ResourceLocateEvent locateEvent{
                resources::ResourceLocateEvent::ResourceType::BINARY,
                uriData
            };
            locateEvent.suggestDirectory(std::filesystem::path(modelFileName).parent_path().string());

            TRY(core->onLocateResource.invoke(locateEvent))
            if (locateEvent.location() == nullptr)
                return errors.error("Resolve buffer URI", fmt::format("File not found: \"{}\"", uriData));
            return locateEvent.location()->readAllBytes();
        }

        constexpr const std::string_view dataURIPrefix{"data:application/octet-stream;base64,"};
        if (!uriData.starts_with(dataURIPrefix)) {
            return errors.error("Understand URI", fmt::format("Unknown prefix!\n", uriData));
        }

        std::string data;
        if (!base64::decode(uriData.substr(std::size(dataURIPrefix)), data)) {
            return errors.error("Decode BASE64", "Failed to decode");
        }
        
        return {std::move(data)};
    }

    [[nodiscard]] base::ErrorOr<GLTFResourceInfo>
    gltfResolveResourceInfo(const nlohmann::json &json, const std::vector<std::string> &buffers, std::size_t bufferViewIndex, const nlohmann::json &accessor) noexcept {
        base::FunctionErrorGenerator errors{ "OpenGLCore", "GLTFLoader" };

        const auto& bufferView = json["bufferViews"][bufferViewIndex];

        const auto bufferIndex = bufferView["buffer"].get<std::size_t>();
        const auto& buffer = buffers[bufferIndex];

        const auto bufferViewByteOffset = bufferView["byteOffset"].get<GLsizeiptr>();

        const auto byteLength = bufferView["byteLength"].get<GLsizeiptr>();
        GLsizeiptr accessorByteOffset{};
        if (auto it = accessor.find("byteOffset"); it != accessor.end())
            accessorByteOffset = it->get<GLsizeiptr>();


        if (accessorByteOffset > byteLength) {
            return errors.error("Check offsets", fmt::format("Accessor ByteOffset > byteLength: {} > {}", accessorByteOffset, byteLength));
        }

        const auto byteOffset = bufferViewByteOffset + accessorByteOffset;

        std::optional<GLsizei> byteStride;
        if (auto it = bufferView.find("byteStride"); it != bufferView.end())
            byteStride = it->get<GLsizei>();

        if (bufferIndex >= std::size(buffers)) {
#ifndef NDEBUG
            std::printf("[GL] GLTFLoader: resolveResourceInfo: bufferIndex(%zu) >= buffersSize(%zu)\n", bufferIndex, std::size(buffers));
#endif // NDEBUG
            return errors.error("Resolve ResourceInfo", "bufferIndex >= bufferSize");
        }

        if (byteOffset < 0 || byteLength <= 0) {
#ifndef NDEBUG
#endif // NDEBUG
            std::printf("[GL] GLTFLoader: resolveResourceInfo: byteOffset(%zu) or byteLength(%zu) is invalid\n", byteOffset, byteLength);
            return errors.error("Resolve ResourceInfo", "bufferOffset < 0 || byteLength <= 0");
        }

        if (static_cast<std::size_t>(bufferViewByteOffset + byteLength) > std::size(buffer)) {
#ifndef NDEBUG
            std::printf("[GL] GLTFLoader: resolveResourceInfo: byteOffset(%zu) + byteLength(%zu) (%zu) >= bufferSize(%zu)\n",
                byteOffset, byteLength, static_cast<std::size_t>(byteOffset + byteLength), std::size(buffer));
#endif
            return errors.error("Resolve ResourceInfo", "Buffer view out of buffer range");
        }

        std::string_view bufferViewData{ buffer.data() + byteOffset, static_cast<std::size_t>(byteLength) };

        if (static_cast<std::size_t>(accessorByteOffset) > std::size(bufferViewData)) {
#ifndef NDEBUG
            fmt::print("Buffer view out of range: buffer.length: {}, accessorByteOffset: {}, bufferViewByteOffset: {}, totalByteOffset: {}, byteLength: {}, bufferView: {}, missingBytes: {}",
                buffer.length(), accessorByteOffset, bufferViewByteOffset, (bufferViewByteOffset + accessorByteOffset), byteLength, buffer.length() - byteLength - bufferViewByteOffset, byteLength + accessorByteOffset + bufferViewByteOffset - buffer.length());
#endif // NDEBUG
            return errors.error("Resolve ResourceInfo", "Buffer view out of buffer range");
        }

        return GLTFResourceInfo{
            accessor,
            buffer,
            bufferView,
            byteOffset,
            byteLength - accessorByteOffset,
            byteStride,
            bufferViewData.substr(static_cast<std::size_t>(accessorByteOffset))
        };
    }

    [[nodiscard]] static base::ErrorOr<GLTFResourceInfo>
    gltfResolveResourceInfo(std::size_t accessorIndex, const nlohmann::json &json, const std::vector<std::string> &buffers) noexcept {
        const auto &accessor = json["accessors"][accessorIndex];
        return gltfResolveResourceInfo(json, buffers, accessor["bufferView"].get<std::size_t>(), accessor);
    }

    [[nodiscard]] static base::Error
    gltfAnalyzeMesh(GLTFInformation &information, const nlohmann::json &primitive) noexcept {
        base::FunctionErrorGenerator errors{"OpenGLCore", "GLCore/GLTFLoader"};

        const auto positionAccessor = primitive["attributes"]["POSITION"].get<std::size_t>();
        TRY_GET_VARIABLE(positionResource, gltfResolveResourceInfo(positionAccessor, information.json, information.buffers))

        const auto positionComponentType = convertComponentType(positionResource.accessor["componentType"]);

        const auto size = componentTypeGetDataTypeSize(positionComponentType.get());
        static_cast<void>(size);
        
        return base::Error::success();
    }

    template<typename T>
    [[nodiscard]] static base::ErrorOr<GLuint>
    gltfUploadVertexBufferObject(GLuint attributeLocation, base::ArrayView<T> data, ComponentType componentType, 
        std::size_t componentCount, std::optional<GLsizei> stride) noexcept {
        base::FunctionErrorGenerator errors{"OpenGLCore", "GLTFLoader"};
        
        GLuint bufferObject;
        glGenBuffers(1, &bufferObject);
        if (glGetError() != GL_NO_ERROR)
            return errors.error("glGenBuffers", "GL_ERROR");

        glBindBuffer(GL_ARRAY_BUFFER, bufferObject);
        glVertexAttribPointer(attributeLocation, static_cast<GLint>(componentCount), static_cast<GLenum>(componentType), GL_FALSE, stride.value_or(0), nullptr);
        
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(data.size() * sizeof(T)), data.data(), GL_STATIC_DRAW);

        const auto err = glGetError();
        if (err != GL_NO_ERROR)
            return errors.error("glBufferData", "GL_ERROR");

        return bufferObject;
    }

    [[nodiscard]] static base::Error
        gltfGenerateEBO(GLTFInformation& information, const nlohmann::json& primitive) {
        base::FunctionErrorGenerator errors{ "OpenGLCore", "GLTFCore" };
        auto accessorIndexIt = primitive.find("indices");
        if (accessorIndexIt == primitive.end()) {
            information.ebo = 0;
            return base::Error::success();
        }

        const auto accessorIndex = accessorIndexIt->get<std::size_t>();
        TRY_GET_VARIABLE(resource, gltfResolveResourceInfo(accessorIndex, information.json, information.buffers))

        assert(!resource.byteStride.has_value());
        assert(!resource.bufferView.contains("target") || resource.bufferView["target"].get<int>() == 34963);

        const auto &accessorType = resource.accessor["type"].get<std::string>();
        if (accessorType != "SCALAR") {
            std::printf("[GL] GLTFLoader: unexpected bufferView.type for EBO: \"%s\"\n", accessorType.c_str());
            return errors.error("Check accessor type", "Unexpected value: not a SCALAR");
        }

        TRY_GET_VARIABLE(componentType, convertComponentType(resource.accessor["componentType"]))

        information.eboType = static_cast<GLenum>(componentType);
        information.indexCount = resource.accessor["count"].get<std::size_t>() ;

        glGenBuffers(1, &information.ebo);
        if (glGetError() != GL_NO_ERROR) {
            std::puts("[GL] GLTFLoader: couldn't generate buffers for EBO");
            return errors.error("glGenBuffers for EBO", "GL Error (todo)");
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, information.ebo);

        const auto *dataBegin = &resource.buffer[static_cast<std::size_t>(resource.byteOffset)];
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, resource.byteLength, dataBegin, GL_STATIC_DRAW);

#if 0
        switch (componentType) {
            case ComponentType::UNSIGNED_SHORT: {
                const auto *data = reinterpret_cast<const unsigned short *>(dataBegin);
                const auto indexCount = resource.byteLength / sizeof(unsigned short);
                for (std::size_t i = 0; i < indexCount; ++i) {
                    if (data[i] > information.vertices.size()) {
                        std::printf("Invalid EBO entry @ %llu value %hu\n", i, data[i]);
                        unsigned short highestIndex = data[i];
                        std::size_t indicesTooHigh{ 1 };
                        for (std::size_t j = i + 1; j < indexCount; ++j) {
                            if (data[j] < information.vertices.size())
                                break;
                            ++indicesTooHigh;
                            if (data[j] > highestIndex)
                                highestIndex = data[j];
                        }

                        return errors.error("Verify EBO", fmt::format(
                            "Index out of bounds: \nindex={} (#{})\nvertices.size={}\nindexCount={}\nhighestIndex: {}\nindicesTooHigh: {}", 
                            data[i], i,
                            information.vertices.size(),
                            indexCount,
                            highestIndex,
                            indicesTooHigh
                        ));
                    }
                }
                break;
            }
            case ComponentType::UNSIGNED_INT: {
                const auto *data = reinterpret_cast<const unsigned int *>(dataBegin);
                for (std::size_t i = 0; i < resource.byteLength / sizeof(unsigned int); ++i) {
                    if (data[i] >= information.vertices.size()) {
                        std::printf("Invalid EBO entry @ %llu value %u\n", i, data[i]);
                        return errors.error("Verify EBO", "Index out of bounds");
                    }
                }
                break;
            }
            default:
                assert(false);
        }
#else
        static_cast<void>(componentType);
#endif

        auto err = glGetError();
        if (err == GL_NO_ERROR)
            return base::Error::success();
        std::printf("[GL] GLTFLoader: failed copy GLTF to EBO data: %zu\n", static_cast<std::size_t>(err));
        return errors.error("Load EBO", "GL Error (todo)");
    }

    [[nodiscard]] static bool
    gltfGenerateVBO(GLTFInformation &information, const nlohmann::json &primitive, GLuint attributeLocation) {
        const auto accessorIndex = primitive["attributes"]["POSITION"].get<std::size_t>();
        const auto resource = gltfResolveResourceInfo(accessorIndex, information.json, information.buffers);
        if (resource.failed()){
            std::printf("[GL] GLTFLoader: failed to resolve resource info for VBO\n");
            return false;
        }

        const auto &accessorType = resource->accessor["type"].get<std::string>();
        if (accessorType != "VEC3") {
            std::printf("[GL] GLTFLoader: unexpected bufferView.type for VBO: \"%s\"\n", accessorType.c_str());
            return false;
        }

        glGenBuffers(1, &information.vbo);
        if (glGetError() != GL_NO_ERROR) {
            std::puts("[GL] GLTFLoader: couldn't generate buffers for VBO");
            return false;
        }

        if (resource->accessor["componentType"].get<GLenum>() != GL_FLOAT) {
            std::puts("[GL] GLTFLoader: invalid VBO accessor.componentType");
            return false;
        }

        glBindBuffer(GL_ARRAY_BUFFER, information.vbo);
        glVertexAttribPointer(attributeLocation, 3, GL_FLOAT, GL_FALSE, resource->byteStride.value_or(0), nullptr);

#ifdef GLTF_VERBOSE_DEBUG
        std::printf("   GLTF VBO offset=%llu byteLength=%llu length=%llu\n",
            static_cast<std::size_t>(resource->byteOffset), static_cast<std::size_t>(resource->byteLength),
            static_cast<std::size_t>(resource->byteLength) / sizeof(math::Vector3f));
#endif // GLTF_VERBOSE_DEBUG

        information.vertices = {
            static_cast<const math::Vector3f *>(
                static_cast<const void *>(
                    resource->buffer.data() + resource->byteOffset
                )
            ),
            static_cast<std::uint32_t>(resource->byteLength / sizeof(math::Vector3f))
        };
        glBufferData(GL_ARRAY_BUFFER, resource->byteLength, information.vertices.data(), GL_STATIC_DRAW);

        auto err = glGetError();
        if (err == GL_NO_ERROR)
            return true;
        std::printf("[GL] GLTFLoader: failed copy GLTF to VBO data: %zu\n", static_cast<std::size_t>(err));
        return false;
    }

    [[nodiscard]] static bool
    gltfGenerateTBO(GLTFInformation &information, const nlohmann::json &primitive, GLuint attributeLocation) {
        const auto accessorIndex = primitive["attributes"]["TEXCOORD_0"].get<std::size_t>();
        const auto resource = gltfResolveResourceInfo(accessorIndex, information.json, information.buffers);
        if (resource.failed()){
            std::printf("[GL] GLTFLoader: failed to resolve resource info for TBO\n");
            return false;
        }

        const auto &accessorType = resource->accessor["type"].get<std::string>();
        if (accessorType != "VEC2") {
            std::printf("[GL] GLTFLoader: unexpected bufferView.type for TBO: \"%s\"\n", accessorType.c_str());
            return false;
        }

        glGenBuffers(1, &information.tbo);
        if (glGetError() != GL_NO_ERROR) {
            std::puts("[GL] GLTFLoader: couldn't generate buffers for TBO");
            return false;
        }

        if (resource->accessor["componentType"].get<GLenum>() != GL_FLOAT) {
            std::puts("[GL] GLTFLoader: invalid TBO accessor.componentType");
            return false;
        }

        glBindBuffer(GL_ARRAY_BUFFER, information.tbo);
        glVertexAttribPointer(attributeLocation, 2, GL_FLOAT, GL_FALSE, resource->byteStride.value_or(0), nullptr);

        const auto *dataBegin = &resource->buffer[static_cast<std::size_t>(resource->byteOffset)];

#ifdef GLTF_VERBOSE_DEBUG
        std::printf("   GLTF TBO offset=%llu byteLength=%llu length=%llu\n",
            static_cast<std::size_t>(resource->byteOffset), static_cast<std::size_t>(resource->byteLength),
            static_cast<std::size_t>(resource->byteLength) / sizeof(math::Vector2f));
#endif // GLTF_VERBOSE_DEBUG

        const auto elementCount = static_cast<std::uint32_t>(resource->byteLength / sizeof(math::Vector2f));
        const auto *elementBegin = reinterpret_cast<const math::Vector2f *>(dataBegin);
        information.textureData = {elementBegin, elementCount};
        std::vector<math::Vector2f> textureData{elementBegin, elementBegin + elementCount};
        assert(elementCount > 0);
        assert(elementBegin != nullptr);

#if 0
        for (auto &coordinate : textureData) {
            coordinate.y() = 1 - coordinate.y();
        }
#endif

        glBufferData(GL_ARRAY_BUFFER, resource->byteLength, textureData.data(), GL_STATIC_DRAW);

        auto err = glGetError();
        if (err == GL_NO_ERROR)
            return true;
        std::printf("[GL] GLTFLoader: failed copy GLTF to TBO data: %zu\n", static_cast<std::size_t>(err));
        return false;
    }

    [[nodiscard]] static bool
    generateSurfaceNormals(GLTFInformation information, const nlohmann::json &primitive, GLuint attributeLocation) noexcept {
        assert(!primitive["attributes"].contains("NORMAL"));

        const auto positionAccessor = primitive["attributes"]["POSITION"].get<std::size_t>();
        const auto positionResource = gltfResolveResourceInfo(positionAccessor, information.json, information.buffers);
        if (positionResource.failed())
            return false;

        information.generatedNormals = std::vector<math::Vector3f>(information.vertices.size());
        for (std::size_t i = 0; i < information.generatedNormals.size(); ++i) {
            const auto v0 = information.vertices[i];
            const auto v1 = information.vertices[i + 1];
            const auto v2 = information.vertices[i + 2];

            math::Vector3f a{
                v1.x() - v0.x(),
                v1.y() - v0.y(),
                v1.z() - v0.z()
            };

            math::Vector3f b{
                v2.x() - v0.x(),
                v2.y() - v0.y(),
                v2.z() - v0.z()
            };
            
            information.generatedNormals[i] = a.cross(b).normalize();
        }

        glBindBuffer(GL_ARRAY_BUFFER, information.nbo);
#ifdef GLTF_VERBOSE_DEBUG
        std::printf("normal attribLocation=%u\n", attributeLocation);
#endif // GLTF_VERBOSE_DEBUG
        glVertexAttribPointer(attributeLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        information.normals = { static_cast<math::Vector3f*>(static_cast<void*>(information.generatedNormals.data())), information.generatedNormals.size() / 3 };
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(information.normals.size()) * sizeof information.normals[0], information.normals.data(), GL_STATIC_DRAW);

        auto err = glGetError();
        if (err == GL_NO_ERROR)
            return true;
        std::printf("[GL] GLTFLoader: failed copy generated surface normals to NBO: %zu\n", static_cast<std::size_t>(err));
        return false;
    }

    [[nodiscard]] static bool
    gltfGenerateNBO(GLTFInformation &information, const nlohmann::json &primitive, GLuint attributeLocation) {
        glGenBuffers(1, &information.nbo);
        if (glGetError() != GL_NO_ERROR) {
            std::puts("[GL] GLTFLoader: couldn't generate buffers for NBO");
            return false;
        }

        const auto &attribs = primitive["attributes"];
        if (attribs.find("NORMAL") == std::cend(attribs)) {
            //std::printf("[GL] GLTFLoader: warning: node \"%s\" in file \"%s\" doesn't contains normal data!\n", nodeName, std::string(information.name).c_str());
            //// No normal data in this Mesh

            //for (auto it = attribs.begin(); it != attribs.end(); ++it) {
            //    fmt::print("Key: {}\n", it.key());
            //}

            //std::printf("[GL] GLTFLoader: generating surface normals...\n");

            return generateSurfaceNormals(information, primitive, attributeLocation);
        }

        const auto accessorIndex = attribs["NORMAL"].get<std::size_t>();
        const auto resource = gltfResolveResourceInfo(accessorIndex, information.json, information.buffers);
        if (resource.failed()){
            std::printf("[GL] GLTFLoader: failed to resolve resource info for NBO\n");
            return false;
        }

        const auto &accessorType = resource->accessor["type"].get<std::string>();
        if (accessorType != "VEC3") {
            std::printf("[GL] GLTFLoader: unexpected bufferView.type for NBO: \"%s\"\n", accessorType.c_str());
            return false;
        }

        if (resource->accessor["componentType"].get<GLenum>() != GL_FLOAT) {
            std::puts("[GL] GLTFLoader: invalid NBO accessor.componentType");
            return false;
        }

#ifdef GLTF_VERBOSE_DEBUG
        std::printf("   GLTF NBO offset=%llu byteLength=%llu length=%llu\n",
            static_cast<std::size_t>(resource->byteOffset), static_cast<std::size_t>(resource->byteLength),
            static_cast<std::size_t>(resource->byteLength) / sizeof(math::Vector3f));
#endif // GLTF_VERBOSE_DEBUG

        glBindBuffer(GL_ARRAY_BUFFER, information.nbo);
        glVertexAttribPointer(attributeLocation, 3, GL_FLOAT, GL_FALSE, resource->byteStride.value_or(0), nullptr);

        const auto *dataBegin = &resource->buffer[static_cast<std::size_t>(resource->byteOffset)];
        information.normals = {reinterpret_cast<const math::Vector3f*>(dataBegin), resource->byteLength / sizeof(math::Vector3f)};
        glBufferData(GL_ARRAY_BUFFER, resource->byteLength, dataBegin, GL_STATIC_DRAW);

        auto err = glGetError();
        if (err == GL_NO_ERROR)
            return true;
        std::printf("[GL] GLTFLoader: failed copy GLTF to NBO data: %zu\n", static_cast<std::size_t>(err));
        return false;
    }

    [[nodiscard]] base::ErrorOr<std::optional<std::size_t>>
    gltfResolveAlbedoTexture(const nlohmann::json &material) noexcept {
        base::FunctionErrorGenerator errors{"OpenGLCore", "GLTFLoader"};
        auto pbrMetallicRoughnessIt = material.find("pbrMetallicRoughness");
        if (pbrMetallicRoughnessIt == material.end())
            return {std::nullopt}; // "[no_material_pbr_metallic_roughness]"

        auto baseColorTextureIt = pbrMetallicRoughnessIt->find("baseColorTexture");
        if (baseColorTextureIt == pbrMetallicRoughnessIt->end())
            return {std::nullopt}; //"[no_pbr_metallic_roughness_base_color_texture]"

        auto indexIt = baseColorTextureIt->find("index");
        if (indexIt == baseColorTextureIt->end()) {
            //"[no_base_color_texture_index]"
            return errors.error("Resolve albedo texture", "No baseColorTexture.index, this is required by the GLTF 2.0 specification.");
        }

        return {indexIt->get<std::size_t>()};
    }

    [[nodiscard]] const nlohmann::json *
    gltfResolveNormalMapTexture(const nlohmann::json &material) noexcept {
        auto normalTextureIt = material.find("normalTexture");
        if (normalTextureIt == material.end())
            return nullptr;

        auto indexIt = normalTextureIt->find("index");
        if (indexIt == normalTextureIt->end())
            return nullptr;

        return &*indexIt;
    }

    [[nodiscard]] static math::Transformation
    gltfParseTransformation(const nlohmann::json &node) {
        math::Transformation transformation{};

        if (const auto scaleIt = node.find("scale"); scaleIt != node.end()) {
            transformation.scaling = math::Vector3f{
                (*scaleIt)[0].get<float>(),
                (*scaleIt)[1].get<float>(),
                (*scaleIt)[2].get<float>(),
            };
        }

        if (const auto rotationIt = node.find("rotation"); rotationIt != node.end()) {
            transformation.rotation = math::Vector3f{
                (*rotationIt)[0].get<float>(),
                (*rotationIt)[1].get<float>(),
                (*rotationIt)[2].get<float>(),
            };
        }

        if (const auto translationIt = node.find("translation"); translationIt != node.end()) {
            transformation.translation = math::Vector3f{
                (*translationIt)[0].get<float>(),
                (*translationIt)[1].get<float>(),
                (*translationIt)[2].get<float>(),
            };
        }

        return transformation;
    }

    [[nodiscard]] static base::Error
    gltfGenerateSkinningInformation(GLTFInformation &information, const nlohmann::json &node, const Renderer &renderer) noexcept {
        base::FunctionErrorGenerator errors{"OpenGLCore", "GLCore/GLTFLoader"};

        auto nodeSkinIt = node.find("skin");
        if (nodeSkinIt == node.end())
            return errors.error("Load skin information", "No node.skin");

        static_cast<void>(information);
        static_cast<void>(renderer);
        return base::Error::success();
    }

    struct GLTFTextureLoadInfo {
        std::string_view fileName;

        GLTFInformation &information;
        const nlohmann::json &node;
        const nlohmann::json &mesh;
        const nlohmann::json &primitive;

        Core &glCore;

        const nlohmann::json *material;
        const nlohmann::json *jsonTextures;
    };

    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-sampler
    [[nodiscard]] static base::ErrorOr<resources::TextureSampler>
    gltfLoadTextureSampler(const GLTFTextureLoadInfo& texLoadInfo, const nlohmann::json& textureJson) noexcept {
        base::FunctionErrorGenerator errors{ "OpenGLCore", "GLTFLoader" };
        resources::TextureSampler sampler{};

        auto samplerIndexIt = textureJson.find("sampler");
        if (samplerIndexIt == textureJson.end())
            return { sampler };

        if (!samplerIndexIt->is_number())
            return errors.error("Verify texture sampler index", "Not a number");

        if (!samplerIndexIt->is_number_unsigned())
            return errors.error("Verify texture sampler index", "Index is negative");

        const auto samplerIndex = samplerIndexIt->get<std::size_t>();
        const auto& json = texLoadInfo.information.json;
        const auto samplersIt = json.find("samplers");
        if (samplersIt == json.end())
            return errors.error("Find sampler", "<root>.samplers missing");

        if (samplersIt->size() <= samplerIndex)
            return errors.error("Find sampler", fmt::format("Sampler Index {} out of bounds, samplers.length: {}", samplerIndex, samplersIt->size()));

        // The properties of a sampler object aren't required.
        // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-sampler
        const auto &samplerJson = (*samplersIt)[samplerIndex];

        // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#_sampler_magfilter
        if (auto magFilterIt = samplerJson.find("magFilter"); magFilterIt != samplerJson.end()) {
            if (!magFilterIt->is_number_integer())
                return errors.error("Parse \"magFilter\"", fmt::format("sampler.magFilter should be of type integer, but isn't: {}", *magFilterIt));
            switch (const auto value = magFilterIt->get<int>()) {
                case 9728:
                    sampler.setMagnificationFilter(resources::TextureFilter::NEAREST);
                    break;
                case 9729:
                    sampler.setMagnificationFilter(resources::TextureFilter::LINEAR);
                    break;
            default:
                return errors.error("Parse \"magFilter\"", fmt::format("Unknown value: {}", value));
            }
        }

        // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#_sampler_minfilter
        if (auto minFilterIt = samplerJson.find("minFilter"); minFilterIt != samplerJson.end()) {
            if (!minFilterIt->is_number_integer())
                return errors.error("Parse \"minFilter\"", fmt::format("sampler.minFilter should be of type integer, but isn't: {}", *minFilterIt));
            switch (const auto value = minFilterIt->get<int>()) {
                case 9728: // NEAREST
                    sampler.setMinifyingFilter(resources::TextureFilter::NEAREST);
                    break;
                case 9729: // LINEAR
                    sampler.setMinifyingFilter(resources::TextureFilter::LINEAR);
                    break;
                case 9984: // NEAREST_MIPMAP_NEAREST
                case 9986: // NEAREST_MIPMAP_LINEAR
                    sampler.setGenerateMipMaps(true);
                    sampler.setMinifyingFilter(resources::TextureFilter::NEAREST);
                    break;
                case 9985: // LINEAR_MIPMAP_NEAREST
                case 9987: // LINEAR_MIPMAP_LINEAR
                    sampler.setGenerateMipMaps(true);
                    sampler.setMinifyingFilter(resources::TextureFilter::LINEAR);
                    break;
                default:
                    return errors.error("Parse \"minFilter\"", fmt::format("Unknown value: {}", value));
            }
        }

        // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#_sampler_wraps
        if (auto wrapSIt = samplerJson.find("wrapS"); wrapSIt != samplerJson.end()) {
            if (!wrapSIt->is_number_integer())
                return errors.error("Parse \"wrapS\"", fmt::format("sampler.wrapS should be of type integer, but isn't: {}", *wrapSIt));
            switch (const auto value = wrapSIt->get<int>()) {
                case 33071: // CLAMP_TO_EDGE
                    sampler.setUCoordinateWrappingMode(resources::TextureCoordinateWrappingMode::CLAMP_TO_EDGE);
                    break;
                case 33648: // MIRRORED_REPEAT
                    sampler.setUCoordinateWrappingMode(resources::TextureCoordinateWrappingMode::REPEAT_MIRRORED);
                    break;
                case 10497: // REPEAT
                    sampler.setUCoordinateWrappingMode(resources::TextureCoordinateWrappingMode::REPEAT);
                    break;
                default:
                    return errors.error("Parse \"wrapS\"", fmt::format("Unknown value: {}", value));
            }
        } else
            sampler.setUCoordinateWrappingMode(resources::TextureCoordinateWrappingMode::REPEAT);

        //
        if (auto wrapTIt = samplerJson.find("wrapT"); wrapTIt != samplerJson.end()) {
            if (!wrapTIt->is_number_integer())
                return errors.error("Parse \"wrapT\"", fmt::format("sampler.wrapS should be of type integer, but isn't: {}", *wrapTIt));
            switch (const auto value = wrapTIt->get<int>()) {
                case 33071: // CLAMP_TO_EDGE
                    sampler.setVCoordinateWrappingMode(resources::TextureCoordinateWrappingMode::CLAMP_TO_EDGE);
                    break;
                case 33648: // MIRRORED_REPEAT
                    sampler.setVCoordinateWrappingMode(resources::TextureCoordinateWrappingMode::REPEAT_MIRRORED);
                    break;
                case 10497: // REPEAT
                    sampler.setVCoordinateWrappingMode(resources::TextureCoordinateWrappingMode::REPEAT);
                    break;
                default:
                    return errors.error("Parse \"wrapT\"", fmt::format("Unknown value: {}", value));
            }
        } else
            sampler.setVCoordinateWrappingMode(resources::TextureCoordinateWrappingMode::REPEAT);

        return sampler;
    }       

    [[nodiscard]] base::Error
    gltfLoadTexture(const GLTFTextureLoadInfo &texLoadInfo, std::size_t textureIndex, resources::ModelDescriptor &modelDescriptor, resources::TextureSlot textureSlot) noexcept {
        auto &texture = texLoadInfo.jsonTextures->at(textureIndex);

        TRY_GET_VARIABLE(sampler, gltfLoadTextureSampler(texLoadInfo, texture))

        auto sourceIt = texture.find("source");
        if (sourceIt == texture.end())
            // "[no_texture_source]"
            return base::Error::success();

        auto imagesIt = texLoadInfo.information.json.find("images");
        if (imagesIt == texLoadInfo.information.json.end())
            // "[no_json_images]"
            return base::Error::success();

        return texLoadInfo.information.imageLoader.subscribeImageLoad(sourceIt->get<std::size_t>(),
            [textureSlot, glCore = &texLoadInfo.glCore, modelDescriptor = &modelDescriptor, sampler = std::move(sampler)]
                (io::image::BulkImageLoader::ImageRequestTag, io::image::BulkImageLoader::Image& loadedImage) {
                TRY_GET_VARIABLE(textureDescriptor, glCore->createTexture(resources::TextureInput{ loadedImage.view(), resources::TextureSampler(sampler) }))
                //fmt::print("Attaching texture {:p} to modelDescriptor {:p}\n", static_cast<const void *>(textureDescriptor), static_cast<const void*>(modelDescriptor));
                assert(modelDescriptor->textureAttachment(textureSlot) == nullptr);
                modelDescriptor->attachTexture(textureSlot, textureDescriptor);
                return base::Error::success();
            }
        );

        //std::unique_ptr<resources::ResourceLocation> resourceLocation;

        //if (auto bufferViewIt = image.find("bufferView"); bufferViewIt != image.end()) {
        //    TRY_GET_VARIABLE(resourceInfo, gltfResolveResourceInfo(texLoadInfo.information.json, texLoadInfo.information.buffers, bufferViewIt->get<std::size_t>(), *bufferViewIt))
        //    resourceLocation = std::make_unique<resources::MemoryNonOwningResourceLocation>(
        //        base::ArrayView{ resourceInfo.bufferPart.data(), resourceInfo.bufferPart.size() }, false
        //    );
        //} else if (auto uriIt = image.find("uri"); uriIt != image.end()) {
        //    if (!uriIt->is_string()) {
        //        return errors.error("Load image URI", fmt::format("URI is not a string: {}", uriIt->dump()));
        //    }

        //    const auto sv = uriIt->get<std::string_view>();
        //    TRY_GET_VARIABLE(buffer, gltfLoadBuffer(sv, &texLoadInfo.glCore, texLoadInfo.information.name))

        //    resourceLocation = std::make_unique<resources::MemoryOwningResourceLocation>(
        //        std::vector<char>(buffer.begin(), buffer.end()), false
        //    );
        //} else if (auto imageNameIt = image.find("name"); imageNameIt != image.end()) {
        //    resources::ResourceLocateEvent resourceLocateEvent{
        //        resources::ResourceLocateEvent::ResourceType::IMAGE,
        //        imageNameIt->get<std::string_view>()
        //    };

        //    resourceLocateEvent.suggestDirectory(std::filesystem::path(texLoadInfo.fileName).parent_path().string());

        //    texLoadInfo.glCore.onLocateResource.invoke(resourceLocateEvent).displayErrorMessageBox();

        //    resourceLocation = std::move(resourceLocateEvent.location());
        //} else {
        //    assert(false);
        //    return base::Error("OpenGLCore", "GLTFLoader", "Parse GLTF image element", "Missing resource fields");
        //}

        //assert(resourceLocation != nullptr);

        //TRY_GET_VARIABLE(request, texLoadInfo.information.imageLoader.requestImageLoad(std::move(resourceLocation)))
        //return texLoadInfo.information.imageLoader.attachDeferredCallback(request,
        //    [textureSlot, glCore = &texLoadInfo.glCore, modelDescriptor = &modelDescriptor, sampler = std::move(sampler)]
        //(io::image::BulkImageLoader::ImageRequestTag, io::image::BulkImageLoader::Image& loadedImage) {
        //        TRY_GET_VARIABLE(textureDescriptor, glCore->createTexture(resources::TextureInput{ loadedImage.view(), resources::TextureSampler(sampler) }))
        //            //fmt::print("Attaching texture {:p} to modelDescriptor {:p}\n", static_cast<const void *>(textureDescriptor), static_cast<const void*>(modelDescriptor));
        //            assert(modelDescriptor->textureAttachment(textureSlot) == nullptr);
        //        modelDescriptor->attachTexture(textureSlot, textureDescriptor);
        //        return base::Error::success();
        //    }
        //);
    }

    [[nodiscard]] base::Error
    gltfLoadTextures(GLTFTextureLoadInfo &texLoadInfo, resources::ModelDescriptor &modelDescriptor) noexcept {
        auto materialIndexIt = texLoadInfo.primitive.find("material");
        if (materialIndexIt == std::end(texLoadInfo.primitive))
            // "[no_mesh_primitives_0_material]"
            return base::Error::success();

        assert(materialIndexIt->is_number());
        auto materialIndex = materialIndexIt->get<unsigned int>();

        auto materialsIt = texLoadInfo.information.json.find("materials");
        if (materialsIt == std::end(texLoadInfo.information.json))
            // "[no_json_materials]"
            return base::Error::success();

        auto texturesIt = texLoadInfo.information.json.find("textures");
        if (texturesIt == texLoadInfo.information.json.end())
            // "[no_json_textures]"
            return base::Error::success();

        texLoadInfo.jsonTextures = &*texturesIt;
        texLoadInfo.material = &materialsIt->at(materialIndex);

#ifdef GLTF_NO_TEXTURES
        static_cast<void>(modelDescriptor);
#else // GLTF_NO_TEXTURES
        if (auto *indexIt = gltfResolveNormalMapTexture(*texLoadInfo.material)) {
            TRY(gltfLoadTexture(texLoadInfo, indexIt->get<unsigned int>(), modelDescriptor, resources::TextureSlot::NORMAL_MAP))
        }

        TRY_GET_VARIABLE(albedoTextureIndex, gltfResolveAlbedoTexture(*texLoadInfo.material));
        if (albedoTextureIndex.has_value()) {
            TRY(gltfLoadTexture(texLoadInfo, albedoTextureIndex.value(), modelDescriptor, resources::TextureSlot::ALBEDO))
        }
#endif // GLTF_NO_TEXTURES

        return base::Error::success();
    }

    [[nodiscard]] static base::Error
    gltfCalculateTangentsAndBitangents(const GLTFInformation &information,
            ModelGeometryDescriptor &geometryDescriptor, 
            AttributeLocations attributeLocations) noexcept {
        std::vector<math::Vector3f> tangents{};
        std::vector<math::Vector3f> bitangents{};

        tangents.reserve(information.vertices.size());
        bitangents.reserve(information.vertices.size());

        const auto count = information.vertices.size();
        for (std::size_t i = 0; i < count; i += 3) {
            const auto &v0 = information.vertices[i];
            const auto &v1 = information.vertices[i + 1];
            const auto &v2 = information.vertices[i + 2];

            const auto &uv0 = information.textureData[i];
            const auto &uv1 = information.textureData[i + 1];
            const auto &uv2 = information.textureData[i + 2];

            const auto deltaVertices1 = v1 - v0;
            const auto deltaVertices2 = v2 - v0;

            const auto deltaUV1 = uv1 - uv0;
            const auto deltaUV2 = uv2 - uv0;

            const auto r = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV1.y() * deltaUV2.x());
            auto tangent = (deltaVertices1 * deltaVertices2.y() - deltaVertices2 * deltaVertices1.y()) * r;
            auto bitangent = (deltaVertices2 * deltaVertices1.x() - deltaVertices1 * deltaVertices2.x()) * r;

            tangents.push_back(tangent);
            tangents.push_back(tangent);
            tangents.push_back(tangent);

            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
        }

        glEnableVertexAttribArray(attributeLocations.tangent);
        glEnableVertexAttribArray(attributeLocations.bitangent);

        const base::ArrayView tangentsView{tangents.data(), static_cast<std::uint32_t>(tangents.size())};
        TRY_GET_VARIABLE(tangentBufferObject, gltfUploadVertexBufferObject(attributeLocations.tangent, tangentsView, ComponentType::FLOAT, 3, std::nullopt))

        const base::ArrayView bitangentsView{bitangents.data(), static_cast<std::uint32_t>(bitangents.size())};
        TRY_GET_VARIABLE(bitangentBufferObject, gltfUploadVertexBufferObject(attributeLocations.bitangent, bitangentsView, ComponentType::FLOAT, 3, std::nullopt))

        geometryDescriptor.setTangentAndBitangentBufferObjects(tangentBufferObject, bitangentBufferObject);

        return base::Error::success();
    }

    [[nodiscard]] static base::Error
    gltfLoadTangentBufferAndCalculateBitangents(const nlohmann::json &tangentJson, const GLTFInformation &information, ModelGeometryDescriptor &geometryDescriptor, const AttributeLocations &attributeLocations) noexcept {
        base::FunctionErrorGenerator errors{"OpenGLCore", "GLTFLoader"};

        TRY_GET_VARIABLE(resourceInfo, gltfResolveResourceInfo(tangentJson.get<std::size_t>(), information.json, information.buffers))

        TRY_GET_VARIABLE(componentType, convertComponentType(resourceInfo.accessor["componentType"]))
        if (componentType != ComponentType::FLOAT)
            return errors.error("Verify accessor information", fmt::format("Component type of TANGENT buffer isn't FLOAT, but: {}", toString(componentType)));

        if (auto accessorTypeIt = resourceInfo.accessor.find("type"); accessorTypeIt != resourceInfo.accessor.end()) {
            const auto accessorType = accessorTypeIt->get<std::string_view>();
            if (accessorType != "VEC4") {
                return errors.error("Verify accessor information", fmt::format("Accessor type of TANGENT buffer isn't VEC4, but: {}", accessorType));
            }
        } else
            return errors.error("Verify accessor information", fmt::format("Accessor type missing, accessor: {}", resourceInfo.accessor.dump()));

        const base::ArrayView tangentsFromFile(reinterpret_cast<const math::Vector4f*>(resourceInfo.bufferPart.data()), resourceInfo.bufferPart.size() / sizeof math::Vector4f);

        if (information.normals.size() != tangentsFromFile.size()) {
            return errors.error("Verify sizes", fmt::format("Normals size ({}) != tangents size ({})", information.normals.size(), tangentsFromFile.size()));
        }

        std::vector<math::Vector3f> tangents(tangentsFromFile.size());
        std::vector<math::Vector3f> bitangents(tangentsFromFile.size());
        for (std::size_t i = 0; i < tangentsFromFile.size(); ++i) {
            const auto &tangent = tangentsFromFile[i];
            const auto &normal = information.normals[i];

            tangents[i] = tangent.xyz();
            bitangents[i] = normal.cross(tangent.xyz()) * tangent.w();
        }

        TRY_GET_VARIABLE(tangentBufferObject, gltfUploadVertexBufferObject(attributeLocations.tangent, base::ArrayView(tangents.data(), tangents.size()), ComponentType::FLOAT, 3, std::nullopt))
        TRY_GET_VARIABLE(bitangentBufferObject, gltfUploadVertexBufferObject(attributeLocations.tangent, base::ArrayView(bitangents.data(), bitangents.size()), ComponentType::FLOAT, 3, std::nullopt))

        geometryDescriptor.setTangentAndBitangentBufferObjects(tangentBufferObject, bitangentBufferObject);
        return base::Error::success();
    }

    [[nodiscard]] static base::Error
    gltfCheckNonMeshNodes(ecs::Scene &scene, GLTFInformation &information, const nlohmann::json &node) noexcept {
        base::FunctionErrorGenerator errors{"OpenGLCore", "GLTFLoader"};

        auto nodeExtensionsIt = node.find("extensions");
        if (nodeExtensionsIt == std::end(node))
            return base::Error::success();

        // https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_lights_punctual/README.md
        if (auto it = nodeExtensionsIt->find("KHR_lights_punctual"); it != nodeExtensionsIt->end()) {
            auto lightIt = it.value().find("light");
            if (lightIt == it.value().end())
                return errors.error("KHR_lights_punctual extension parsing", "\"light\" key missing");

            if (!lightIt->is_number())
                return errors.error("KHR_lights_punctual extension parsing", "\"light\" key isn't a number");

            if (!lightIt->is_number_unsigned())
                return errors.error("KHR_lights_punctual extension parsing", "\"light\" key is should be unsigned");

            const auto lightIndex = lightIt->get<unsigned int>();

            if (information.lightExtensionLights == nullptr)
                return errors.error("KHR_lights_punctual extension parsing", "json.extensions.KHR_lights_punctual missing");

            if (lightIndex >= information.lightExtensionLights->size())
                return errors.error("KHR_lights_punctual extension parsing", "json.extensions.KHR_lights_punctual out of bounds");

            auto &light = (*information.lightExtensionLights)[lightIndex];
            auto typeIt = light.find("type");
            if (typeIt == light.end())
                return errors.error("KHR_lights_punctual extension parsing", "json.extensions.KHR_lights_punctual doesn't contain a type");

            math::Vector3f color{1.0f, 1.0f, 1.0f};
            if (auto colorIt = light.find("color"); colorIt != light.end())
                color = math::Vector3f{
                    (*colorIt)[0].get<float>(),
                    (*colorIt)[1].get<float>(),
                    (*colorIt)[2].get<float>(),
                };

            auto radius = 3.0f;
            auto intensity = 0.2f;
            auto attenuationConstant = 0.0f;
            auto attenuationLinear = 0.0f;
            auto attenuationExponent = 1.0f;

            std::string name;
            if (auto nameIt = node.find("name"); nameIt != node.end())
                name = nameIt->get<std::string>();
            else
                name = "PointLight";

            auto transform = gltfParseTransformation(node);
            scene.entityList().add(std::make_unique<ecs::PointLight>(
                std::move(name), transform, color, radius, 
                intensity,
                attenuationConstant, 
                attenuationLinear, 
                attenuationExponent
            ));
            
            std::printf("[GLTFScene] Loaded entity type=PointLight name=\"%s\"\n",
                    scene.entityList().data().back()->name().c_str());

            float sphereScale = 0.01f;
            transform.scaling = math::Vector3f{sphereScale, sphereScale, sphereScale};
            scene.entityList().add(std::make_unique<ecs::Entity>(
                "PointLight_Sphere", information.sphereModel, transform
            ));

            std::printf("[GLTFScene] Loaded entity type=Entity name=\"%s\"\n",
                    scene.entityList().data().back()->name().c_str());
        }

        return base::Error::success();
    }

    [[nodiscard]] base::ErrorOr<std::unique_ptr<ecs::Scene>>
    Core::loadGLTFScene(std::string_view fileName) noexcept {
        base::FunctionErrorGenerator errors{"OpenGLCore", "GLCore/GLTFLoader"};

        std::ifstream stream{std::string(fileName)};
        if (!stream)
            return errors.error("Open GLTF scene file", "File could not be opened.");

        resources::ModelDescriptor *sphereModel;
        {
            TRY_GET_VARIABLE(sphere, createSphere(18, 36, 1.0f))
            /*TRY_GET_VARIABLE(texture, 
                createTexture(
                    resources::TextureInput{
                        std::make_unique<resources::FileResourceLocation>(
                            std::filesystem::path(
                                "C:\\Users\\tager\\Pictures\\VisualStudioAvatars.png"
                            )
                        )
                    }
                )
            );*/

            m_modelDescriptors.push_back(std::make_unique<resources::ModelDescriptor>(
                sphere //, texture
            ));
            sphereModel = m_modelDescriptors.back().get();
        }

        resources::ModelGeometry geometry{};

        nlohmann::json loadedJson;
        try {
            stream >> loadedJson;

            if (std::empty(loadedJson["nodes"]))
                return errors.error("Parse GLTF", "File doesn't contain any nodes");

            if (!loadedJson["buffers"].is_array())
                return errors.error("Parse GLTF", "buffers type != array");

            // End of validation

            
            /*io::image::BulkImageLoader imageLoader{};
            imageLoader.onFinishLoad += [&](io::image::BulkImageLoader::FinishLoadEvent& event) {
                event.error().displayErrorMessageBox();
                return base::Error::success();
            };*/

            std::vector<std::string> loadedBuffers;
            loadedBuffers.reserve(loadedJson["buffers"].size());
            for (const auto &buffer : loadedJson["buffers"]) {
                TRY_GET_VARIABLE(bufferString, 
                    gltfLoadBuffer(buffer["uri"].get<std::string_view>(), this, fileName))
                
                    loadedBuffers.push_back(std::move(bufferString));
            }

            Context context{ *this, fileName, std::move(loadedJson), std::move(loadedBuffers) };
            const auto &json = context.json();
            const auto& buffers = context.buffers();

            ImageLoader imageLoader{context};

            const auto imagesPreLoadBegin = std::chrono::high_resolution_clock::now();
            TRY(imageLoader.preLoadAll())
            const auto imagesPreLoadEnd = std::chrono::high_resolution_clock::now();
            fmt::print("[GLTF] Loaded preloaded images took {} ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(imagesPreLoadEnd - imagesPreLoadBegin).count());


            auto scene = std::make_unique<ecs::Scene>(ecs::EntityList{});
            const nlohmann::json *lightExtensionLights{};

            if (auto extensionsIt = json.find("extensions"); extensionsIt != json.end()) {
                if (auto it = extensionsIt->find("KHR_lights_punctual"); it != extensionsIt->end())
                    if (auto lightsIt = it->find("lights"); lightsIt != it->end())
                        lightExtensionLights = &lightsIt.value();
            }

            const auto parsingEntitiesBegin = std::chrono::high_resolution_clock::now();

            std::size_t nodeIndex{};
            for (const auto &node : json["nodes"]) {
                GLTFInformation information{ fileName, json, buffers, sphereModel, context, imageLoader };
                information.lightExtensionLights = lightExtensionLights;
                auto meshIt = node.find("mesh");
                if (meshIt == std::cend(node)) {
                    TRY(gltfCheckNonMeshNodes(*scene, information, node))
                    continue;
                }

                const auto &mesh = json["meshes"][meshIt->get<std::size_t>()];
                std::string nodeName{};
                if (auto nameIt = node.find("name"); nameIt != node.end())
                    nodeName = nameIt->get<std::string>();
#ifdef GLTF_ANNOUNCE_PARSING
                const auto nodeBeginParsing = std::chrono::high_resolution_clock::now();
                fmt::print("GLTFLoader> Node #{} named \"{}\"\n", nodeIndex, nodeName);
#endif // GLTF_ANNOUNCE_PARSING

#ifdef GLTF_ANNOUNCE_PARSING
                std::size_t primitiveIndex{};
#endif // GLTF_ANNOUNCE_PARSING
                for (const auto &primitive : mesh["primitives"]) {
#ifdef GLTF_ANNOUNCE_PARSING
                    const auto primitiveBegin = std::chrono::high_resolution_clock::now();
                    fmt::print("GLTFLoader> Primitive #{} of node #{} \"{}\"\n", primitiveIndex++, nodeIndex, nodeName);
#endif // GLTF_ANNOUNCE_PARSING
                    if (auto it = primitive.find("mode"); it != primitive.end()) {
                        if (!it->is_number())
                            return errors.error("Load mesh", "mesh.primitive.mode isn't an integer.");
                        const auto value = it->get<int>();
                        if (value != 4) {
                            return errors.error("Load mesh", fmt::format("mesh.primitive.mode incorrect: {} != 4 (TRIANGLES)", value));
                        }
                    }

                    const auto &primitiveAttributes = primitive["attributes"];

                    GLuint vao{};
                    glGenVertexArrays(1, &vao);
                    glBindVertexArray(vao);

                    information.hasSkin = false;
                    // node.find("skin") != node.end();

                    CapabilitiesRequired capabilities{
                        .hasSkin = information.hasSkin,
                    };
                    const auto attributeLocations = m_renderer->attributeLocations(capabilities);
                    glEnableVertexAttribArray(attributeLocations.position);
                    glEnableVertexAttribArray(attributeLocations.normal);
                    glEnableVertexAttribArray(attributeLocations.textureCoordinates);

                    if (auto error = gltfAnalyzeMesh(information, primitive))
                        return error;

                    if (!gltfGenerateVBO(information, primitive, attributeLocations.position))
                        return errors.error("Generate VBO", "Unknown Error");

                    TRY(gltfGenerateEBO(information, primitive))

                    if (!gltfGenerateTBO(information, primitive, attributeLocations.textureCoordinates))
                        return errors.error("Generate TBO", "Unknown Error");

                    if (!gltfGenerateNBO(information, primitive, attributeLocations.normal))
                        return errors.error("Generate NBO", "Unknown Error");

                    if (information.hasSkin)
                        TRY(gltfGenerateSkinningInformation(information, node, *m_renderer))

                    m_geometryDescriptors.push_back(
                        std::make_unique<ModelGeometryDescriptor>(static_cast<GLsizei>(information.vertices.size() * 3), vao,
                            information.vbo, information.ebo, information.tbo,
                            information.nbo, static_cast<GLsizei>(information.indexCount), information.eboType));

                    auto* geometryDescriptor = m_geometryDescriptors.back().get();
                    resources::ModelDescriptor modelDescriptor{ geometryDescriptor };

                    if (modelDescriptor.normalMapTextureDescriptor() != nullptr) {
                        if (auto tangent = primitiveAttributes.find("TANGENT"); tangent != primitiveAttributes.end()) {
                            TRY(gltfLoadTangentBufferAndCalculateBitangents(*tangent, information, *geometryDescriptor, attributeLocations))
                            assert(geometryDescriptor->tangentBufferObject() != 0);
                            assert(geometryDescriptor->bitangentBufferObject() != 0);
                        }

                        if (geometryDescriptor->tangentBufferObject() == 0) {
                            assert(!primitiveAttributes.contains("TANGENT"));
                            TRY(gltfCalculateTangentsAndBitangents(information, *geometryDescriptor, attributeLocations))
                        }
                    }

                    TRY_GET_VARIABLE(model, uploadModelDescriptor(std::forward<resources::ModelDescriptor>(modelDescriptor)))

                    GLTFTextureLoadInfo textureLoadInfo {
                        fileName, information, node, mesh, primitive, * this
                    };
                    if (auto error = gltfLoadTextures(textureLoadInfo, const_cast<resources::ModelDescriptor&>(*model)))
                        return error;

                    auto* entity = scene->entityList().create(std::string(nodeName), std::move(model), gltfParseTransformation(node));
                    if (entity == nullptr)
                        return errors.error("Create Entity", "Unknown Error");

#ifdef GLTF_ANNOUNCE_PARSING
                    const auto primitiveEndParsing = std::chrono::high_resolution_clock::now();
                    fmt::print("   Took: {} ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(primitiveEndParsing - primitiveBegin).count());
#endif // GLTF_ANNOUNCE_PARSING

#ifdef GLTF_VERBOSE_DEBUG
                    std::printf("[GLTFScene] Loaded entity type=Entity name=\"%s\"\n",
                        scene->entityList().data().back()->name().c_str());
#endif // GLTF_VERBOSE_DEBUG
                }

#ifdef GLTF_ANNOUNCE_PARSING
                const auto nodeEndParsing = std::chrono::high_resolution_clock::now();
                fmt::print("[GLTF] Loaded entity in {} ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(nodeEndParsing - nodeBeginParsing).count());
#endif // GLTF_ANNOUNCE_PARSING
                ++nodeIndex;
            }

            const auto parsingEntitiesEnd = std::chrono::high_resolution_clock::now();
            fmt::print("[GLTF] Parsed entities took {} ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(parsingEntitiesEnd - parsingEntitiesBegin).count());

            const auto textureBegin = std::chrono::high_resolution_clock::now();
            TRY(imageLoader.waitForAll())
            const auto textureEnd = std::chrono::high_resolution_clock::now();
            fmt::print("[GLTF] Loaded textures in {} ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(textureEnd - textureBegin).count());

            return scene;
        } catch (std::exception &ex) {
            static std::string buffer = ex.what();
            return errors.error("Parse GLTF", buffer);
        }
    }

} // namespace gle
