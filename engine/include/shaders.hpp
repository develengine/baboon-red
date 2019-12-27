#pragma once

#include "eng.h"

#include <string>


namespace Shader {

void generate(
    u32 &program,
    u32 &vertexShader,
    u32 &fragmentShader,

    const std::string &vert,
    const std::string &frag
);

std::string load(std::string path);

void destroy(u32 program, u32 vertexShader, u32 fragmentShader);

};
