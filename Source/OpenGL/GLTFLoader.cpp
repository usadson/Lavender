/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "GLCore.hpp"

#include <fstream>
#include <optional>

#include <cassert>

#include <nlohmann/json.hpp>

#include "Source/Base/ArrayView.hpp"
#include "ThirdParty/base64.hpp"

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

    [[nodiscard]] inline constexpr std::size_t
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
        const nlohmann::json &json;
        const std::vector<std::string> &buffers;

        GLuint ebo{};
        GLenum eboType{};
        std::size_t indexCount{};

        GLuint vbo{};
        GLuint tbo{};
    };

    struct GLTFResourceInfo {
        const nlohmann::json &accessor;
        const std::string &buffer;
        const nlohmann::json &bufferView;

        GLsizeiptr byteOffset;
        GLsizeiptr byteLength;
    };

    /**
     * Return views of 0 are error conditions, since no buffers of length 0 are
     * allowed anyway.
     */
    [[nodiscard]] inline static std::string
    gltfLoadBuffer(std::string_view uriData, std::size_t byteLength) noexcept {
        constexpr const std::string_view dataURIPrefix{"data:application/octet-stream;base64,"};
        if (!uriData.starts_with(dataURIPrefix)) {
            std::printf("[GL] GLTFLoader: failed to understand URI: \"%s\"\n", std::string(uriData).c_str());
            return {};
        }

        std::string data;
        if (!base64::decode(uriData.substr(std::size(dataURIPrefix)), data)) {
            std::puts("[GL] GLTFLoader: failed to decode base64 of data URI");
            return {};
        }

        // TODO
        static_cast<void>(byteLength);

        return data;
    }

    [[nodiscard]] inline static std::optional<GLTFResourceInfo>
    gltfResolveResourceInfo(std::size_t accessorIndex, const nlohmann::json &json, const std::vector<std::string> &buffers) noexcept {
        const auto &accessor = json["accessors"][accessorIndex];

        const auto bufferViewIndex = accessor["bufferView"].get<std::size_t>();
        const auto &bufferView = json["bufferViews"][bufferViewIndex];

        const auto bufferIndex = bufferView["buffer"].get<std::size_t>();
        const auto &buffer = buffers[bufferIndex];

        const auto byteLength = bufferView["byteLength"].get<GLsizeiptr>();
        const auto byteOffset = bufferView["byteOffset"].get<GLsizeiptr>();

        if (bufferIndex >= std::size(buffers)) {
            std::printf("[GL] GLTFLoader: resolveResourceInfo: bufferIndex(%zu) >= buffersSize(%zu)\n", bufferIndex, std::size(buffers));
            return std::nullopt;
        }

        if (byteOffset < 0 || byteLength <= 0) {
            std::printf("[GL] GLTFLoader: resolveResourceInfo: byteOffset(%zu) or byteLength(%zu) is invalid\n", byteOffset, byteLength);
            return std::nullopt;
        }

        if (static_cast<std::size_t>(byteOffset + byteLength) > std::size(buffer)) {
            std::printf("[GL] GLTFLoader: resolveResourceInfo: byteOffset(%zu) + byteLength(%zu) (%zu) >= bufferSize(%zu)\n",
                byteOffset, byteLength, static_cast<std::size_t>(byteOffset + byteLength), std::size(buffer));
            return std::nullopt;
        }

        return GLTFResourceInfo{
            accessor,
            buffer,
            bufferView,
            byteOffset,
            byteLength,
        };
    }

    [[nodiscard]] inline static bool
    gltfGenerateEBO(GLTFInformation &information, const nlohmann::json &mesh) {
        const auto accessorIndex = mesh["primitives"][0]["indices"].get<std::size_t>();
        const auto resource = gltfResolveResourceInfo(accessorIndex, information.json, information.buffers);
        if (!resource.has_value()) {
            std::printf("[GL] GLTFLoader: failed to resolve resource info!\n");
            return false;
        }

        const auto &accessorType = resource->accessor["type"].get<std::string>();
        if (accessorType != "SCALAR") {
            std::printf("[GL] GLTFLoader: unexpected bufferView.type for EBO: \"%s\"\n", accessorType.c_str());
            return false;
        }

        information.eboType = resource->accessor["componentType"].get<GLenum>();
        information.indexCount = resource->accessor["count"].get<std::size_t>() ;

        glGenBuffers(1, &information.ebo);
        if (glGetError() != GL_NO_ERROR) {
            std::puts("[GL] GLTFLoader: couldn't generate buffers for EBO");
            return false;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, information.ebo);

        const auto *dataBegin = &resource->buffer[static_cast<std::size_t>(resource->byteOffset)];
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, resource->byteLength, dataBegin, GL_STATIC_DRAW);

        base::ArrayView<const unsigned short> indices{reinterpret_cast<const unsigned short *>(dataBegin), static_cast<std::uint32_t>(static_cast<std::size_t>(resource->byteLength) / sizeof(unsigned short))};
        for (auto it = std::begin(indices); it != std::end(indices); it += 3) {
            std::printf("face> %hu %hu %hu\n", it[0], it[1], it[2]);
        }

        std::cout << Hexdump(reinterpret_cast<const void *>(dataBegin), static_cast<std::size_t>(resource->byteLength)) << std::flush;

        auto err = glGetError();
        if (err == GL_NO_ERROR)
            return true;
        std::printf("[GL] GLTFLoader: failed copy GLTF to EBO data: %zu\n", static_cast<std::size_t>(err));
        return false;
    }

    [[nodiscard]] inline static bool
    gltfGenerateVBO(GLTFInformation &information, const nlohmann::json &mesh, GLuint attributeLocation) {
        const auto accessorIndex = mesh["primitives"][0]["attributes"]["POSITION"].get<std::size_t>();
        const auto resource = gltfResolveResourceInfo(accessorIndex, information.json, information.buffers);
        if (!resource.has_value()){
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
        std::printf("attribLocation=%u\n", attributeLocation);
        glVertexAttribPointer(attributeLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        const auto *dataBegin = &resource->buffer[static_cast<std::size_t>(resource->byteOffset)];
        glBufferData(GL_ARRAY_BUFFER, resource->byteLength, dataBegin, GL_STATIC_DRAW);

        base::ArrayView<const float> vertices{reinterpret_cast<const float *>(dataBegin), static_cast<std::uint32_t>(static_cast<std::size_t>(resource->byteLength) / sizeof(float))};
        for (auto it = std::begin(vertices); it != std::end(vertices); it += 3) {
            std::printf("vertex> %f %f %f\n", static_cast<double>(it[0]), static_cast<double>(it[1]), static_cast<double>(it[2]));
        }

        auto err = glGetError();
        if (err == GL_NO_ERROR)
            return true;
        std::printf("[GL] GLTFLoader: failed copy GLTF to VBO data: %zu\n", static_cast<std::size_t>(err));
        return false;
    }

    [[nodiscard]] inline static bool
    gltfGenerateTBO(GLTFInformation &information, const nlohmann::json &mesh, GLuint attributeLocation) {
        const auto accessorIndex = mesh["primitives"][0]["attributes"]["TEXCOORD_0"].get<std::size_t>();
        const auto resource = gltfResolveResourceInfo(accessorIndex, information.json, information.buffers);
        if (!resource.has_value()){
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
        std::printf("attribLocation=%u\n", attributeLocation);
        glVertexAttribPointer(attributeLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        const auto *dataBegin = &resource->buffer[static_cast<std::size_t>(resource->byteOffset)];
        glBufferData(GL_ARRAY_BUFFER, resource->byteLength, dataBegin, GL_STATIC_DRAW);

        auto err = glGetError();
        if (err == GL_NO_ERROR)
            return true;
        std::printf("[GL] GLTFLoader: failed copy GLTF to TBO data: %zu\n", static_cast<std::size_t>(err));
        return false;
    }

    [[nodiscard]] resources::ModelGeometryDescriptor *
    Core::loadGLTFModelGeometry(std::string_view fileName) noexcept {
        std::ifstream stream{std::string(fileName)};
        if (!stream) {
            std::printf("[GL] GLTFLoader: failed to open file: \"%s\"\n", std::string(fileName).c_str());
            return nullptr;
        }
        resources::ModelGeometry geometry{};

        nlohmann::json json;
        try {
            stream >> json;

            if (json["meshes"].size() != 1) {
                std::printf("[GL] GLTFLoader: Meshes count isn't 1 (val=%lu)\n", static_cast<unsigned long>(std::size(json["meshes"])));
                return nullptr;
            }

            if (!json["buffers"].is_array()) {
                std::printf("[GL] GLTFLoader: buffers type != array\n");
                return nullptr;
            }

            auto &mesh = json["meshes"][0];

            std::vector<std::string> buffers;
            buffers.reserve(json["buffers"].size());
            for (const auto &buffer : json["buffers"]){
                auto bufferString = gltfLoadBuffer(buffer["uri"].get<std::string>(), buffer["byteLength"].get<std::size_t>());
                if (std::empty(bufferString)) {
                    std::puts("[GL] GLTFLoader: failed to load URI buffer");
                    return nullptr;
                }
                buffers.push_back(std::move(bufferString));
            }

            GLuint vao{};
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glEnableVertexAttribArray(m_shaderAttribPosition);
            glEnableVertexAttribArray(m_shaderAttribTextureCoordinates);

            GLTFInformation information{json, buffers};

            if (!gltfGenerateEBO(information, mesh)) {
                std::puts("[GL] GLTFLoader: failed to generate EBO!");
                return nullptr;
            }

            if (!gltfGenerateVBO(information, mesh, m_shaderAttribPosition)) {
                std::puts("[GL] GLTFLoader: failed to generate VBO!");
                return nullptr;
            }

            if (!gltfGenerateTBO(information, mesh, m_shaderAttribTextureCoordinates)) {
                std::puts("[GL] GLTFLoader: failed to generate VBO!");
                return nullptr;
            }

            m_geometryDescriptors.push_back(std::make_unique<ModelGeometryDescriptor>(
                vao,
                information.vbo,
                information.ebo,
                information.tbo,
                0,
                static_cast<GLsizei>(information.indexCount),
                information.eboType
            ));
            return m_geometryDescriptors.back().get();
        } catch (...) {
            std::printf("[GL] GLTFLoader: JSON exception caught\n");
            return nullptr;
        }
    }

} // namespace gle
