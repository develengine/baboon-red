#include "shaders.hpp"

#include <glad/glad.h>

#include <iostream>
#include <fstream>
#include <streambuf>

GraphicShader::GraphicShader(std::string vert, std::string frag) {
    int succes;
    char infoLog[512];

    const char *vertexSource = vert.c_str();
    const char *fragmentSource = frag.c_str();

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &succes);
    if (!succes) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Failed to compile vertex shader! Error:\n" << infoLog << '\n';
    }
    
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &succes);
    if (!succes) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Failed to compile vertex shader! Error:\n" << infoLog << '\n';
    }
    
    programId = glCreateProgram();
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &succes);
    if (!succes) {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        std::cerr << "Failed to link shader program! Error:\n" << infoLog << '\n';
    }
}

uint32_t GraphicShader::id() {
    return programId;
}

int GraphicShader::getUniform(std::string name) {
    return glGetUniformLocation(programId, name.c_str());
}

void GraphicShader::bindUBO(std::string name, uint32_t binding) {
    uint32_t uboIndex = glGetUniformBlockIndex(programId, name.c_str());
    glUniformBlockBinding(programId, uboIndex, binding);
}

void GraphicShader::use() {
    glUseProgram(programId);
}

void GraphicShader::useNone() {
    glUseProgram(0);
}

std::string GraphicShader::load(const std::string path) {
    std::ifstream file(path);
    std::string str;
    
    file.seekg(0, std::ios::end);   
    str.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    
    str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return str;
}

void GraphicShader::free() {
    if (alive) {
        alive = false;
        glDetachShader(programId, vertexShader);
        glDeleteShader(vertexShader);
        glDetachShader(programId, fragmentShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(programId);
    }
}

GraphicShader::~GraphicShader() {
    free();
}
