#version 460 core

layout(location = 0) in vec2 o_texCoords;

layout(location = 0) out vec4 f_color;

uniform vec4 u_textColor;
uniform vec4 u_backColor;
uniform sampler2D u_charSheet;

void main() {
    float brightness = texture(u_charSheet, o_texCoords).r;
    f_color = u_textColor * brightness + u_backColor * (1 - brightness);
}

