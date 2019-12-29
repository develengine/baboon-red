#version 460 core

layout(location = 0) in vec2 i_position;
layout(location = 1) in vec2 i_texCoords;
layout(location = 2) in uint i_data;

layout(location = 0) out vec2 o_texCoords;

uniform int u_wrapLen;
uniform vec4 u_transform;

void main()
{
    uint character = (i_data << 24) >> 24;

    vec2 texOffset = vec2(i_texCoords.x / 32.0, i_texCoords.y / 8.0);
    vec2 charOffset = vec2(float(character % 32) / 32.0, float(character / 32) / 8.0);
    o_texCoords = texOffset + charOffset;

    vec2 wrapOffset = vec2(u_transform.z * (gl_InstanceID % u_wrapLen), u_transform.w * (gl_InstanceID / u_wrapLen) * -1.0);
    vec2 positionOffset = u_transform.xy + wrapOffset;
    gl_Position = vec4(i_position * u_transform.zw + positionOffset, 0.0, 1.0);
}

