#pragma once

#include <string>

#include <stdint.h>

class GraphicShader {
    uint32_t vertexShader = 0;
    uint32_t fragmentShader = 0;
    uint32_t programId = 0;

    bool alive = true;

public:
    GraphicShader(const std::string &vert, const std::string &frag);
    GraphicShader() = default;

    void generate(const std::string &vert, const std::string &frag);

    uint32_t id();
    int getUniform(std::string name);
    void bindUBO(std::string name, uint32_t binding);
    void use();

    static void useNone();
    static std::string load(std::string path);

    void free();
    ~GraphicShader();
};
