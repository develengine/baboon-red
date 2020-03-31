#pragma once

#include "eng.h"

namespace Engine {

enum VaoIdentifiers
{
    CLASSIC_VAO,
    RECTANGLE_VAO,
    CUBE_VAO,
    TEXT_VAO,
    DUMMY_VAO,

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

enum ProgramIdentifiers
{
    CLASSIC_PROGRAM,
    PLAIN_PROGRAM,
    RECTANGLE_PROGRAM,
    TEXT_PROGRAM,
    RECT_COL_PROGRAM,
    LINE_PROGRAM,

    PROGRAM_COUNT
};

enum ShaderIdentifiers
{
    CLASSIC_VERTEX_SHADER,
    CLASSIC_FRAGMENT_SHADER,

    PLAIN_VERTEX_SHADER,
    PLAIN_FRAGMENT_SHADER,

    RECTANGLE_VERTEX_SHADER,
    RECTANGLE_FRAGMENT_SHADER,

    TEXT_VERTEX_SHADER,
    TEXT_FRAGMENT_SHADER,

    RECT_COL_VERTEX_SHADER,
    RECT_COL_FRAGMENT_SHADER,

    LINE_VERTEX_SHADER,
    LINE_FRAGMENT_SHADER,

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

    LINE_VERT0_UNIFORM,
    LINE_VERT1_UNIFORM,
    LINE_COLOR_UNIFORM,

    UNIFORM_COUNT
};

extern u32 vaos[VAO_COUNT];
extern u32 buffers[BUFFER_COUNT];
extern u32 textures[TEXTURE_COUNT];
extern u32 programs[PROGRAM_COUNT];
extern u32 shaders[SHADER_COUNT];
extern int uniforms[UNIFORM_COUNT];

}

#define ENG_VAO(NAME) ( Engine::vaos     [ Engine::NAME ## _VAO     ] )
#define ENG_BUF(NAME) ( Engine::buffers  [ Engine::NAME ## _BUFFER  ] )
#define ENG_TEX(NAME) ( Engine::textures [ Engine::NAME ## _TEXTURE ] )
#define ENG_SHD(NAME) ( Engine::shaders  [ Engine::NAME ## _SHADER  ] )
#define ENG_PRG(NAME) ( Engine::programs [ Engine::NAME ## _PROGRAM ] )
#define ENG_UNI(NAME) ( Engine::uniforms [ Engine::NAME ## _UNIFORM ] )

