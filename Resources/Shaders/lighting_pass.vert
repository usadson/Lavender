/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#version 330 core

layout (location = 0) in vec2 vertex_position;

out vec2 fragment_textureCoordinates;

void main() {
    gl_Position = vec4(vertex_position, 0.0, 1.0);
    fragment_textureCoordinates = vertex_position * 0.5 + 0.5;
}
