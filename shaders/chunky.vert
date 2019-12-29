#version 460 core

layout(location = 0) in vec2 i_position;
layout(location = 1) in vec2 i_texCoords;
layout(location = 2) in uint i_data;

layout(location = 0) out vec2 o_texCoords;

const uint CHUNK_WIDTH    = 32;
const uint TILE_SET_WIDTH = 32;

uniform vec2 u_position;
uniform vec2 u_scale;

void main()
{
    vec2 textureOffset = vec2(float(i_data % TILE_SET_WIDTH), float(i_data / TILE_SET_WIDTH));
    o_texCoords = (i_texCoords + textureOffset) / float(TILE_SET_WIDTH);

    vec2 offset = vec2(float(gl_InstanceID % CHUNK_WIDTH), -float(gl_InstanceID / CHUNK_WIDTH));
    gl_Position = vec4(u_position + ((i_position + offset) / float(CHUNK_WIDTH)) * u_scale, 0.0, 1.0);
}
