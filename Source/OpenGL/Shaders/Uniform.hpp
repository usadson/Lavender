/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Source/Math/Matrix4x4.hpp"
#include "Source/Math/Vector.hpp"

namespace gle {

    class Uniform {
    public:
        using LocationType = int;

        [[nodiscard]] inline constexpr explicit
        Uniform(LocationType location) noexcept
                : m_location(location) {
        }

        [[nodiscard]] inline constexpr LocationType
        location() const noexcept {
            return m_location;
        }

    private:
        LocationType m_location;
    };

#define GLE_UNIFORM_DEF(name, ...) \
    class Uniform##name \
            : public Uniform { \
    public: \
        [[nodiscard]] inline constexpr explicit \
        Uniform##name(Uniform::LocationType location) noexcept \
                : Uniform(location) { \
        } \
        void \
        store(const __VA_ARGS__ &) noexcept; \
    };

    GLE_UNIFORM_DEF(Matrix4, math::Matrix4x4<float>)
    GLE_UNIFORM_DEF(Vector2, math::Vector<float, 2>)
    GLE_UNIFORM_DEF(Vector3, math::Vector<float, 3>)
    GLE_UNIFORM_DEF(Vector4, math::Vector<float, 4>)

} // namespace gle
