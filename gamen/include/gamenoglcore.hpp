#pragma once

#include "eng.h"

namespace Gamen {

enum VaoIdentifiers
{
    CLASSIC_VAO,
    CHUNKY_VAO,

    VAO_COUNT
};

enum BufferIdentifiers
{
    CLASSIC_VBO_BUFFER,
    CLASSIC_IBO_BUFFER,

    CHUNKY_VBO_BUFFER,
    CHUNKY_IBO_BUFFER,
    CHUNKY_DATA_BUFFER,

    BUFFER_COUNT
};

enum TextureIdentifiers
{
    CLASSIC_TEXTURE,

    TEXTURE_COUNT
};

enum ProgramIdentifiers
{
    GAMEN_PROGRAM,
    CHUNKY_PROGRAM,

    PROGRAM_COUNT
};

enum ShaderIdentifiers
{
    GAMEN_VERTEX_SHADER,
    GAMEN_FRAGMENT_SHADER,

    CHUNKY_VERTEX_SHADER,
    CHUNKY_FRAGMENT_SHADER,

    SHADER_COUNT
};

enum UniformIdentifiers
{
    GAMEN_POSITION_UNIFORM,
    GAMEN_SCALE_UNIFORM,
    GAMEN_TEX_OFFSET_UNIFORM,
    GAMEN_TEX_SPAN_UNIFORM,

    CHUNKY_POSITION_UNIFORM,
    CHUNKY_SCALE_UNIFORM,

    UNIFORM_COUNT
};

extern u32 vaos[VAO_COUNT];
extern u32 buffers[BUFFER_COUNT];
extern u32 textures[TEXTURE_COUNT];
extern u32 programs[PROGRAM_COUNT];
extern u32 shaders[SHADER_COUNT];
extern int uniforms[UNIFORM_COUNT];

}

#define VAO(NAME) ( Gamen::vaos     [ Gamen::NAME ## _VAO     ] )
#define BUF(NAME) ( Gamen::buffers  [ Gamen::NAME ## _BUFFER  ] )
#define TEX(NAME) ( Gamen::textures [ Gamen::NAME ## _TEXTURE ] )
#define SHD(NAME) ( Gamen::shaders  [ Gamen::NAME ## _SHADER  ] )
#define PRG(NAME) ( Gamen::programs [ Gamen::NAME ## _PROGRAM ] )
#define UNI(NAME) ( Gamen::uniforms [ Gamen::NAME ## _UNIFORM ] )

