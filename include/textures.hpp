#pragma once

#include <string>
#include <stdint.h>

struct TexParameters {
    uint8_t *data;
    int width, height;
    uint32_t wrapS, wrapT, minFilter, magFilter;
    uint32_t localFormat, deviceFormat, formatType;
};

class Texture {

    uint32_t textureId;

public:
    Texture(const TexParameters &parameters);

    void bind(uint32_t slot, bool activate);

    void free();

    ~Texture();
};
