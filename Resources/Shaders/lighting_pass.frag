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

struct Attenuation {
    float constant;
    float linear;
    float exponent;
};

struct Light {
    vec3 m_position;
    vec3 m_color;
    float m_radius;
    float m_intensity;
    Attenuation m_attenuation;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main() {
    vec3 fragPos = texture(gPosition, fragment_textureCoordinates).rgb;
    vec3 normal = texture(gNormal, fragment_textureCoordinates).rgb;
    vec3 albedo = texture(gAlbedoSpec, fragment_textureCoordinates).rgb;
    //float specular = texture(gAlbedoSpec, fragment_textureCoordinates).a;
    float specular = 1.0;

    // then calculate lighting as usual
    vec3 lighting = albedo * 0.1; // hard-coded ambient component
    vec3 viewDir = normalize(viewPos - fragPos);

    if (normal == vec3(0.0, 0.0, 0.0)) {
        // This normal is invalid, and results in no color.
        outColor = vec4(lighting, 1.0);
        return;
    }

    for (int i = 0; i < NR_LIGHTS; ++i) {
        vec3 lightDir = normalize(lights[i].m_position - fragPos);
        float distance = length(lights[i].m_position - fragPos);

        if (distance < lights[i].m_radius) {
            vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * lights[i].m_color * lights[i].m_intensity;
            float attenuation = lights[i].m_attenuation.constant
                              + lights[i].m_attenuation.linear * distance
                              + lights[i].m_attenuation.exponent * distance * distance
                              + 0.0001;
            lighting += diffuse / attenuation;
        }
    }

    outColor = vec4(lighting, 1.0);
}
