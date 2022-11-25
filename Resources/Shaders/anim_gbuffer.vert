// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#version 150 core

in vec3 position;
in vec3 vertex_normal;
in vec2 vertex_textureCoordinates;
in vec4 vertex_joint;
in vec4 vertex_weight;

out vec3 fragment_position;
out vec3 fragment_normal;
out vec2 fragment_textureCoordinates;
out mat3 fragment_tbnMatrix;

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_jointMatrix[12];

void main() {
    vec4 worldPosition = u_transform * vec4(position, 1.0);

    mat4 skinMatrix = 
        vertex_weight.x * u_jointMatrix[int(vertex_joint.x)] +
        vertex_weight.y * u_jointMatrix[int(vertex_joint.y)] +
        vertex_weight.z * u_jointMatrix[int(vertex_joint.z)] +
        vertex_weight.w * u_jointMatrix[int(vertex_joint.w)];

    gl_Position = u_projection * u_view * skinMatrix * worldPosition;

    fragment_position = worldPosition.xyz;
    fragment_normal = normalize(vertex_normal);
    fragment_textureCoordinates = vertex_textureCoordinates;
}

