#version 460 core

layout(location = 0) in vec2 i_position;

uniform vec2 u_position;
uniform vec2 u_scale;

void main() {
    gl_Position = vec4(i_position * u_scale + u_position, 0.0f, 1.0f);
}

