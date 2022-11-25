// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#version 150 core

in vec3 position;
in vec3 vertex_normal;
in vec2 vertex_textureCoordinates;
in vec3 vertex_tangent;
in vec3 vertex_bitangent;

out vec3 fragment_position;
out vec3 fragment_normal;
out vec2 fragment_textureCoordinates;
out mat3 fragment_tbnMatrix;

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;

void main() {
    vec3 T = normalize(vec3(u_transform * vec4(vertex_tangent, 0.0)));
    vec3 B = normalize(vec3(u_transform * vec4(vertex_bitangent, 0.0)));
    vec3 N = normalize(vec3(u_transform * vec4(vertex_normal, 0.0)));
    fragment_tbnMatrix = mat3(T, B, N);

    vec4 worldPosition = u_transform * vec4(position, 1.0);

    gl_Position = u_projection * u_view * worldPosition;

    fragment_position = worldPosition.xyz;
    fragment_normal = normalize(vertex_normal);
    fragment_textureCoordinates = vertex_textureCoordinates;
}

