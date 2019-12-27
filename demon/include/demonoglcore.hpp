#pragma once

#include "eng.h"

namespace Demon {

enum VaoIdentifiers
{
    CLASSIC_VAO,
    CUBE_VAO,

    VAO_COUNT
};

enum BufferIdentifiers
{
    CLASSIC_VBO_BUFFER,
    CLASSIC_IBO_BUFFER,

    CUBE_VBO_BUFFER,
    CUBE_IBO_BUFFER,

    BUFFER_COUNT
};

enum TextureIdentifiers
{
    CLASSIC_TEXTURE,

    TEXTURE_COUNT
};

enum ProgramIdentifiers
{
    CLASSIC_PROGRAM,
    PLAIN_PROGRAM,

    PROGRAM_COUNT
};

enum ShaderIdentifiers
{
    CLASSIC_VERTEX_SHADER,
    CLASSIC_FRAGMENT_SHADER,

    PLAIN_VERTEX_SHADER,
    PLAIN_FRAGMENT_SHADER,

    SHADER_COUNT
};

enum UniformIdentifiers
{
    UNIFORM_COUNT
};

extern u32 vaos[VAO_COUNT];
extern u32 buffers[BUFFER_COUNT];
extern u32 textures[TEXTURE_COUNT];
extern u32 programs[PROGRAM_COUNT];
extern u32 shaders[SHADER_COUNT];
extern int uniforms[UNIFORM_COUNT];

}

#define VAO(NAME) ( Demon::vaos     [ Demon::NAME ## _VAO     ] )
#define BUF(NAME) ( Demon::buffers  [ Demon::NAME ## _BUFFER  ] )
#define TEX(NAME) ( Demon::textures [ Demon::NAME ## _TEXTURE ] )
#define SHD(NAME) ( Demon::shaders  [ Demon::NAME ## _SHADER  ] )
#define PRG(NAME) ( Demon::programs [ Demon::NAME ## _PROGRAM ] )
#define UNI(NAME) ( Demon::uniforms [ Demon::NAME ## _UNIFORM ] )

