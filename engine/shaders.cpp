#include "shaders.hpp"

#include <glad/glad.h>

#include <iostream>
#include <fstream>
#include <streambuf>

namespace Shader {

void generate(
    u32 &program,
    u32 &vertexShader,
    u32 &fragmentShader,

    const std::string &vert,
    const std::string &frag
) {
    int succes;
    char infoLog[512];

    const char *vertexSource = vert.c_str();
    const char *fragmentSource = frag.c_str();

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &succes);

    if (!succes)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Failed to compile vertex shader! Error:\n" << infoLog << '\n';
    }
    
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &succes);

    if (!succes)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Failed to compile vertex shader! Error:\n" << infoLog << '\n';
    }
    
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &succes);

    if (!succes)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Failed to link shader program! Error:\n" << infoLog << '\n';
    }
}

std::string load(const std::string path)
{
    std::ifstream file(path);
    std::string str;
    
    file.seekg(0, std::ios::end);   
    str.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    
    str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return str;
}

void destroy(u32 program, u32 vertexShader, u32 fragmentShader)
{
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(program);
}

}

