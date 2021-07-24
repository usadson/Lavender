/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#version 330 core

out vec4 outColor;

in vec2 fragment_textureCoordinates;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 m_position;
    vec3 m_color;
    float m_radius;
};

uniform int u_mode;

void main() {
    switch (u_mode) {
        case 0:
            // shouldn't be reached
            discard;
        case 1:
            outColor = normalize(vec4(texture(gPosition, fragment_textureCoordinates).rgb, 1.0));
            break;
        case 2:
            outColor = vec4(texture(gNormal, fragment_textureCoordinates).rgb, 1.0);
            break;
        case 3:
            outColor = vec4(texture(gAlbedoSpec, fragment_textureCoordinates).rgb, 1.0);
            break;
    }
}
