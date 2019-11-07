#pragma once

#include <string>

#include <stdint.h>

class GraphicShader {
    uint32_t vertexShader;
    uint32_t fragmentShader;
    uint32_t programId;

    bool alive = true;

public:
    GraphicShader(std::string vert, std::string frag);

    uint32_t id();
    int getUniform(std::string name);
    void bindUBO(std::string name, uint32_t binding);
    void use();

    static void useNone();
    static std::string load(std::string path);

    void free();
    ~GraphicShader();
};
