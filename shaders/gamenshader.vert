#version 460 core

layout(location = 0) in vec2 i_position;
layout(location = 1) in vec2 i_texCoords;

layout(location = 0) out vec2 o_texCoords;

uniform vec2 u_position;
uniform vec2 u_scale;

uniform vec2 u_texOffset;
uniform vec2 u_texSpan;

void main() {
    gl_Position = vec4(i_position * u_scale + u_position, 0.0f, 1.0f);
    o_texCoords = u_texOffset + i_texCoords * u_texSpan;
}

