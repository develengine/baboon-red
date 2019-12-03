#pragma once

#include <glad/glad.h>
#include "shaders.hpp"
#include "textures.hpp"

enum VaoIdentifiers
{
    CLASSIC_VAO,
    RECTANGLE_VAO,
    TEXT_VAO,

    VAO_COUNT
};

enum BufferIdentifiers
{
    CLASSIC_VBO_BUFFER,
    CLASSIC_IBO_BUFFER,

    RECTANGLE_VBO_BUFFER,

    TEXT_VBO_BUFFER,
    TEXT_STRING_BUFFER,

    BUFFER_COUNT
};

enum TextureIdentifiers
{
    CLASSIC_TEXTURE,
    CHARSHEET_TEXTURE,

    TEXTURE_COUNT
};

enum ShaderIdentifiers
{
    CLASSIC_SHADER,
    RECTANGLE_SHADER,
    TEXT_SHADER,
    RECT_COL_SHADER,

    SHADER_COUNT
};

enum UniformIdentifiers
{
    TEXT_TRANSFORM_UNIFORM,
    TEXT_COLOR_UNIFORM,
    TEXT_WRAP_UNIFORM,

    UNIFORM_COUNT
};

uint32_t vaos[VAO_COUNT];
uint32_t buffers[BUFFER_COUNT];
Texture  textures[TEXTURE_COUNT];
GraphicShader shaders[SHADER_COUNT];
int uniforms[UNIFORM_COUNT];

#define VAO(NAME) ( vaos[ NAME ## _VAO ] )
#define BUF(NAME) ( buffers[ NAME ## _BUFFER ] )
#define TEX(NAME) ( textures[ NAME ## _TEXTURE ] )
#define SHD(NAME) ( shaders[ NAME ## _SHADER ] )
#define UNI(NAME) ( uniforms[ NAME ## _UNIFORM ] )

