#pragma once

#include "eng.h"

#include <string>
#include <stdint.h>

namespace Texture {

struct Parameters
{
    u8 *data;
    int width, height;
    u32 wrapS, wrapT, minFilter, magFilter;
    u32 localFormat, deviceFormat, formatType;
    bool mipmap;
};

void generate(u32 &texture, const Parameters &parameters);

}
