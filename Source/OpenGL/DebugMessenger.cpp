/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * OpenGL® and the oval logo are trademarks or registered trademarks of
 * Hewlett Packard Enterprise in the United States and/or other
 * countries worldwide.
 */

#include "Source/OpenGL/DebugMessenger.hpp"

#include <iostream>
#include <mutex>
#include <string_view>

#include <GL/glew.h>

namespace gle {

    namespace DebugMessenger {

        [[nodiscard]] inline constexpr std::string_view
        translateErrorType(GLenum type) {
            switch (type) {
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "deprecated-behavior";
            case GL_DEBUG_TYPE_ERROR: return "error";
            case GL_DEBUG_TYPE_OTHER: return "other";
            case GL_DEBUG_TYPE_PORTABILITY: return "portability";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "undefined-behavior";
            default:
                return "(invalid)";
            }
        }

        [[nodiscard]] inline constexpr std::string_view
        translateSeverity(GLenum type) {
            switch (type) {
            case GL_DEBUG_SEVERITY_LOW: return "low";
            case GL_DEBUG_SEVERITY_MEDIUM: return "medium";
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "notification";
            case GL_DEBUG_SEVERITY_HIGH: return "high";
            default:
                return "(invalid)";
            }
        }

        [[nodiscard]] inline constexpr std::string_view
        translateSource(GLenum type) {
            switch (type) {
            case GL_DEBUG_SOURCE_API: return "api";
            case GL_DEBUG_SOURCE_OTHER: return "other";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "shader-compiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "third-party";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "window-system";
            default: return "(invalid)";
            }
        }

        std::mutex g_mutex;

        void GLAPIENTRY
        messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                        GLsizei length, const GLchar *message, const void *) {
            const std::lock_guard lock{g_mutex};

            auto &stream = std::cout;
            stream << "\x1b[37m=====[ \x1b[31mOpenGL Implementation Message \x1b[37m]=====\x1b[0m\n";
            stream << "       Type: " << DebugMessenger::translateErrorType(type) << '\n';
            stream << "       Severity: " << DebugMessenger::translateSeverity(severity) << '\n';
            stream << "       Source: " << DebugMessenger::translateSource(source) << '\n';
            stream << "       ID: " << id << '\n';
            stream << "       Message: \"" << std::string_view(message, static_cast<std::size_t>(length)) << "\"\n";
            stream << std::flush;
        }

    } // namespace DebugMessenger

    void
    DebugMessenger::setup() noexcept {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(&DebugMessenger::messageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

} // namespace gle
