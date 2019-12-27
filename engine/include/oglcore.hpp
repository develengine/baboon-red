#pragma once

#include "eng.h"

#include <glad/glad.h>
#include "shaders.hpp"
#include "textures.hpp"

enum VaoIdentifiers
{
    CLASSIC_VAO,
    RECTANGLE_VAO,
    CUBE_VAO,
    TEXT_VAO,

    VAO_COUNT
};

enum BufferIdentifiers
{
    CLASSIC_VBO_BUFFER,
    CLASSIC_IBO_BUFFER,

    CUBE_VBO_BUFFER,
    CUBE_IBO_BUFFER,

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
    PLAIN_SHADER,
    RECTANGLE_SHADER,
    TEXT_SHADER,
    RECT_COL_SHADER,

    SHADER_COUNT
};

enum UniformIdentifiers
{
    TEXT_TRANSFORM_UNIFORM,
    TEXT_COLOR_UNIFORM,
    TEXT_BACKGROUND_UNIFORM,
    TEXT_WRAP_UNIFORM,

    RECTANGLE_POSITION_UNIFORM,
    RECTANGLE_SCALE_UNIFORM,

    RECT_COL_POSITION_UNIFORM,
    RECT_COL_SCALE_UNIFORM,
    RECT_COL_COLOR_UNIFORM,

    UNIFORM_COUNT
};

extern u32 vaos[VAO_COUNT];
extern u32 buffers[BUFFER_COUNT];
extern Texture  textures[TEXTURE_COUNT];
extern GraphicShader shaders[SHADER_COUNT];
extern int uniforms[UNIFORM_COUNT];

#define VAO(NAME) ( vaos[ NAME ## _VAO ] )
#define BUF(NAME) ( buffers[ NAME ## _BUFFER ] )
#define TEX(NAME) ( textures[ NAME ## _TEXTURE ] )
#define SHD(NAME) ( shaders[ NAME ## _SHADER ] )
#define UNI(NAME) ( uniforms[ NAME ## _UNIFORM ] )

