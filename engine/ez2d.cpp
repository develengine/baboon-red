#include "ez2d.hpp"

#include "shaders.hpp"
#include "oglcore.hpp"

#include <glad/glad.h>

namespace EZ2D {

float rectangleVertices[]
{
     1.0f,-1.0f,  1.0f, 1.0f,
     1.0f, 0.0f,  1.0f, 0.0f,
     0.0f, 0.0f,  0.0f, 0.0f,

     0.0f,-1.0f,  0.0f, 1.0f,
     1.0f,-1.0f,  1.0f, 1.0f,
     0.0f, 0.0f,  0.0f, 0.0f
};

void init()
{
    // Vao loading
    glGenVertexArrays(1, &ENG_VAO(RECTANGLE));
    glBindVertexArray(ENG_VAO(RECTANGLE));

    glGenBuffers(1, &ENG_BUF(RECTANGLE_VBO));
    glBindBuffer(GL_ARRAY_BUFFER, ENG_BUF(RECTANGLE_VBO));
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // Shader loading
    Shader::generate(
        ENG_PRG(RECTANGLE),
        ENG_SHD(RECTANGLE_VERTEX), ENG_SHD(RECTANGLE_FRAGMENT),
        Shader::load("shaders/rect.vert"),
        Shader::load("shaders/rect.frag")
    );

    ENG_UNI(RECTANGLE_POSITION) = glGetUniformLocation(ENG_PRG(RECTANGLE), "u_position");
    ENG_UNI(RECTANGLE_SCALE)    = glGetUniformLocation(ENG_PRG(RECTANGLE), "u_scale");

    glUseProgram(ENG_PRG(RECTANGLE));
    glUniform1i(glGetUniformLocation(ENG_PRG(RECTANGLE), "u_sampler"), 0);
    glUseProgram(0);

    Shader::generate(
        ENG_PRG(RECT_COL),
        ENG_SHD(RECT_COL_VERTEX), ENG_SHD(RECT_COL_FRAGMENT),
        Shader::load("shaders/rectcolor.vert"),
        Shader::load("shaders/rectcolor.frag")
    );

    ENG_UNI(RECT_COL_POSITION) = glGetUniformLocation(ENG_PRG(RECT_COL), "u_position");
    ENG_UNI(RECT_COL_SCALE)    = glGetUniformLocation(ENG_PRG(RECT_COL), "u_scale");
    ENG_UNI(RECT_COL_COLOR)    = glGetUniformLocation(ENG_PRG(RECT_COL), "u_color");
}

void close()
{
    Shader::destroy(ENG_PRG(RECTANGLE), ENG_SHD(RECTANGLE_VERTEX), ENG_SHD(RECTANGLE_FRAGMENT));
    Shader::destroy(ENG_PRG(RECT_COL), ENG_SHD(RECT_COL_VERTEX), ENG_SHD(RECT_COL_FRAGMENT));
    glDeleteBuffers(1, &ENG_BUF(RECTANGLE_VBO));
    glDeleteVertexArrays(1, &ENG_VAO(RECTANGLE));
}

}

