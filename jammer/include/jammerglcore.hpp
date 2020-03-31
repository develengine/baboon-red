#pragma once

#include "eng.h"

namespace Jammer {

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

#define VAO(NAME) ( Jammer::vaos     [ Jammer::NAME ## _VAO     ] )
#define BUF(NAME) ( Jammer::buffers  [ Jammer::NAME ## _BUFFER  ] )
#define TEX(NAME) ( Jammer::textures [ Jammer::NAME ## _TEXTURE ] )
#define SHD(NAME) ( Jammer::shaders  [ Jammer::NAME ## _SHADER  ] )
#define PRG(NAME) ( Jammer::programs [ Jammer::NAME ## _PROGRAM ] )
#define UNI(NAME) ( Jammer::uniforms [ Jammer::NAME ## _UNIFORM ] )

