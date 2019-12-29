#version 460 core

layout(location = 0) in vec2 o_texCoords;

layout(location = 0) out vec4 f_color;

uniform sampler2D u_sampler;

void main() {
    f_color = texture(u_sampler, o_texCoords);
//     f_color = vec4(1.0, 0.0, 0.0, 1.0);
}

