#pragma once

#include "eng.h"

#include <cmath>

namespace Models {

struct TexCoordInfo
{
    float ax, ay, bx, by;
    float cx, cy, dx, dy;
    float ex, ey, fx, fy;
    float sm;
};

struct InterleavedModelData
{
    u8 *rawData = nullptr;
    u32 rawSize = 0;

    u32 modelCount = 0;
    char **names = nullptr;

    float **vertices = nullptr;
    u32 *vertexCounts = 0;

    u32 **indices = nullptr;
    u32 *indexCounts = 0;

    char **textures = nullptr;

    void initialize(u32 mc);
    void free();
};


inline float cubeVertices[]
{
     1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
     1.0f,-1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,  0.0f, 0.0f, 1.0f,

    -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f,-1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, 1.0f,-1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f,-1.0f,-1.0f, -1.0f, 0.0f, 0.0f,

    -1.0f, 1.0f,-1.0f,  0.0f, 0.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,  0.0f, 0.0f,-1.0f,
     1.0f, 1.0f,-1.0f,  0.0f, 0.0f,-1.0f,
     1.0f,-1.0f,-1.0f,  0.0f, 0.0f,-1.0f,

     1.0f, 1.0f,-1.0f,  1.0f, 0.0f, 0.0f,
     1.0f,-1.0f,-1.0f,  1.0f, 0.0f, 0.0f,
     1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
     1.0f,-1.0f, 1.0f,  1.0f, 0.0f, 0.0f,

     1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
     1.0f, 1.0f,-1.0f,  0.0f, 1.0f, 0.0f,
    -1.0f, 1.0f,-1.0f,  0.0f, 1.0f, 0.0f,

     1.0f,-1.0f,-1.0f,  0.0f,-1.0f, 0.0f,
    -1.0f,-1.0f,-1.0f,  0.0f,-1.0f, 0.0f,
     1.0f,-1.0f, 1.0f,  0.0f,-1.0f, 0.0f,
    -1.0f,-1.0f, 1.0f,  0.0f,-1.0f, 0.0f
};

inline u32 cubeIndices[]
{
    0, 2, 3,  0, 3, 1,
    4, 6, 7,  4, 7, 5,
    8, 10, 11,  8, 11, 9,
    12, 14, 15,  12, 15, 13,
    16, 18, 19,  16, 19, 17,
    20, 22, 23,  20, 23, 21
};


inline int wrapAround(int x, int y)
{
    int a = abs(x) % y;
    if (x < 0)
    {
        return y - a;
    }
    return a;
}

constexpr u32 nCillinderVC(u32 n, bool texCoords)
{
    return (texCoords ? 48 : 36) * n;
}

constexpr u32 nCillinderIC(u32 n)
{
    return 12 * n - 12;
}

void nCillinder(
    int n,
    float *vertices,
    u32 *indices,
    const TexCoordInfo *texInfo = nullptr
);

void loadInterleavedModel(InterleavedModelData &model, const char *dir);

}
