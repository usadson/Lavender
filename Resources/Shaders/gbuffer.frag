// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#version 330 core

in vec3 fragment_position;
in vec3 fragment_normal;
in vec2 fragment_textureCoordinates;
in mat3 fragment_tbnMatrix;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec4 outAlbedoSpec;

uniform sampler2D texAlbedo;
uniform sampler2D texNormalMap;

void main() {
    outPosition = fragment_position;
    
    //outNormal = normalize(fragment_normal);

    outNormal = texture(texNormalMap, fragment_textureCoordinates).rgb;
    outNormal = outNormal * 2.0 - 1.0;
    outNormal = normalize(fragment_tbnMatrix * outNormal);
    
    vec4 albedoFromTexture = texture(texAlbedo, fragment_textureCoordinates);
    outAlbedoSpec.rgb = albedoFromTexture.rgb;

    if (albedoFromTexture.w <= 0)
        outAlbedoSpec.w = 0;
    else
        outAlbedoSpec.w = 1;
}
