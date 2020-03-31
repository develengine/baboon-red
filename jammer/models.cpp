#include "models.hpp"

#include <cstring>

#include "engutils.hpp"

namespace Models {

void nCillinder(
    int n,
    float *vertices,
    u32 *indices,
    const TexCoordInfo *texInfo
) {
    if (n < 1)
    {
        std::cerr << "Error: Can't create a cillinder with '" << n << "' sides.\n";
        return;
    }

    bool texturing = texInfo != nullptr;
    int vSize = (texturing ? 8 : 6);
    float cx[2], cy[2], lx[2], ly[2], ud[2];
    float seg = 0.f, span = 0.f;
    if (texturing)
    {
        cx[0] = (texInfo->bx + texInfo->ax) * .5f; 
        cx[1] = (texInfo->dx + texInfo->cx) * .5f;
        cy[0] = (texInfo->by + texInfo->ay) * .5f;
        cy[1] = (texInfo->dy + texInfo->cy) * .5f;
        lx[0] = (texInfo->bx - texInfo->ax) * .5f;
        lx[1] = (texInfo->dx - texInfo->cx) * .5f;
        ly[0] = (texInfo->by - texInfo->ay) * .5f;
        ly[1] = (texInfo->dy - texInfo->cy) * .5f;
        ud[0] = texInfo->ey;
        ud[1] = texInfo->fy;
        span = texInfo->fx - texInfo->ex;
        seg = (span / (float)n) * texInfo->sm;
    }

    float angle = (M_PI * 2) / n;

    for (int i = 0; i < n; i++)
    {
        float x = sin(angle * i), z = cos(angle * i);
        const float y[] = { 1,-1 };

        for (int j = 0; j < 2; j++)
        {
            // Tops
            int offset = n * vSize * j + vSize * i;

            vertices[offset]     = x;
            vertices[offset + 1] = y[j];
            vertices[offset + 2] = z;

            vertices[offset + 3] = 0.0f;
            vertices[offset + 4] = y[j];
            vertices[offset + 5] = 0.0f;

            if (texturing)
            {
                vertices[offset + 6] = cx[j] + x * lx[j];
                vertices[offset + 7] = cy[j] + z * ly[j];
            }

            // Sides
            int offset1 = n * vSize * 2 + vSize * j + i * vSize * 4;
            int offset2 = n * vSize * 2 + vSize * j + wrapAround(i * 2 - 1, n * 2) * vSize * 2;

            vertices[offset1]     = x;
            vertices[offset1 + 1] = y[j];
            vertices[offset1 + 2] = z;

            vertices[offset1 + 3] = sin(angle * i + (angle / 2));
            vertices[offset1 + 4] = 0.0f;
            vertices[offset1 + 5] = cos(angle * i + (angle / 2));

            vertices[offset2]     = x;
            vertices[offset2 + 1] = y[j];
            vertices[offset2 + 2] = z;

            vertices[offset2 + 3] = sin(angle * i - (angle / 2));
            vertices[offset2 + 4] = 0.0f;
            vertices[offset2 + 5] = cos(angle * i - (angle / 2));

            if (texturing)
            {
                float segOffset = texInfo->ex + fmod(seg * i, span);
                vertices[offset1 + 6] = segOffset;
                vertices[offset1 + 7] = ud[j];
                vertices[offset2 + 6] = i == 0 ? texInfo->fx : segOffset;
                vertices[offset2 + 7] = ud[j];
            }
        }
    }

    int i = 0;
    for (u32 v = 1; v < n - 1; v++)
    {
        indices[i]     = 0;
        indices[i + 1] = v;
        indices[i + 2] = v + 1;
        i += 3;
    }

    for (u32 v = n * 2 - 1; v > n + 1; v--)
    {
        indices[i]     = n;
        indices[i + 1] = v;
        indices[i + 2] = v - 1;
        i += 3;
    }

    for (u32 j = 0; j < n; j++)
    {
        u32 v = 2 * n + 4 * j;
        indices[i]     = v;
        indices[i + 1] = v + 3;
        indices[i + 2] = v + 2;
        indices[i + 3] = v;
        indices[i + 4] = v + 1;
        indices[i + 5] = v + 3;
        i += 6;
    }
}

void InterleavedModelData::initialize(u32 mc)
{
    modelCount = mc;
    names = new char*[mc];
    vertices = new float*[mc];
    vertexCounts = new u32[mc];
    indices = new u32*[mc];
    indexCounts = new u32[mc];
    textures = new char*[mc];
}

void InterleavedModelData::free()
{
    delete[] names;
    delete[] vertices;
    delete[] vertexCounts;
    delete[] indices;
    delete[] indexCounts;
    delete[] textures;
    delete[] rawData;
}

void loadInterleavedModel(InterleavedModelData &model, const char *dir)
{
    FILE *inFile;
    
    inFile = fopen(dir, "rb");
    fseek(inFile, 0, SEEK_END);
    model.rawSize = ftell(inFile);
    rewind(inFile);
    
    model.rawData = new u8[model.rawSize];
    fread(model.rawData, model.rawSize, 1, inFile);
    fclose(inFile);


    const u8 MODEL_SYMBOL = 7;
    const u8 VERTEX_SYMBOL = 42;
    const u8 INDEX_SYMBOL = 69;
    const u8 TEXTURE_SYMBOL = 96;

    // TODO Add support for different vertex sizes

    float magicFloat = *((float*)(model.rawData));
    u32 magicInt = *((u32*)(&(model.rawData[4])));

    bool floatNative = (magicFloat > 6.6f && magicFloat < 6.7f);
    bool intNative = magicInt == 666;

    if (!intNative)
    {
        eng::swapBytes(&(model.rawData[8]));
    }

    model.initialize(*((u32*)(&(model.rawData[8]))));

    int currentModel = -1;

    u32 position = 12;
    u8 c;

    while (position < model.rawSize)
    {
        if ((c = model.rawData[position]) == MODEL_SYMBOL)
        {
            ++currentModel;
            model.names[currentModel] = (char*)(&(model.rawData[++position]));

            while ((c = model.rawData[position]) != 0)
            {
                ++position;
            }

            ++position;
        }
        else if (c == TEXTURE_SYMBOL)
        {
            model.textures[currentModel] = (char*)(&(model.rawData[++position]));

            while ((c = model.rawData[position]) != 0)
            {
                ++position;
            }

            ++position;
        }
        else if (c == VERTEX_SYMBOL)
        {
            ++position;

            if (!intNative)
            {
                eng::swapBytes(&(model.rawData[position]));
            }

            model.vertexCounts[currentModel] = *((u32*)(&(model.rawData[position])));
            position += 4;
            model.vertices[currentModel] = (float*)(&(model.rawData[position]));

            if (floatNative)
            {
                position += model.vertexCounts[currentModel];
            }
            else
            {
                u32 i;

                for (i = position; i < position + model.vertexCounts[currentModel]; i += 4)
                {
                    eng::swapBytes(&(model.rawData[i]));
                }

                position = i;
            }
        }
        else if (c == INDEX_SYMBOL)
        {
            ++position;

            if (!intNative)
            {
                eng::swapBytes(&(model.rawData[position]));
            }

            model.indexCounts[currentModel] = *((u32*)(&(model.rawData[position])));
            position += 4;
            model.indices[currentModel] = (u32*)(&(model.rawData[position]));
            
            if (intNative)
            {
                position += model.indexCounts[currentModel];
            }
            else
            {
                u32 i;

                for (i = position; i < position + model.indexCounts[currentModel]; i += 4)
                {
                    eng::swapBytes(&(model.rawData[i]));
                }

                position = i;
            }
        }
        else
        {
            std::cerr << "Currupted file: '" << dir << "'\n";
            std::cout << position << '\n';
            exit(-1);
        }
    }
}
}
