#include "ez2d.hpp"

#include "oglcore.hpp"

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
    glGenVertexArrays(1, &VAO(RECTANGLE));
    glBindVertexArray(VAO(RECTANGLE));

    glGenBuffers(1, &BUF(RECTANGLE_VBO));
    glBindBuffer(GL_ARRAY_BUFFER, BUF(RECTANGLE_VBO));
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // Shader loading
    SHD(RECTANGLE).generate(
        GraphicShader::load("shaders/rect.vert"),
        GraphicShader::load("shaders/rect.frag")
    );

    UNI(RECTANGLE_POSITION) = SHD(RECTANGLE).getUniform("u_position");
    UNI(RECTANGLE_SCALE) = SHD(RECTANGLE).getUniform("u_scale");

    SHD(RECTANGLE).use();
    glUniform1i(SHD(RECTANGLE).getUniform("u_sampler"), 0);

    SHD(RECT_COL).generate(
        GraphicShader::load("shaders/rectcolor.vert"),
        GraphicShader::load("shaders/rectcolor.frag")
    );

    UNI(RECT_COL_POSITION) = SHD(RECT_COL).getUniform("u_position");
    UNI(RECT_COL_SCALE) = SHD(RECT_COL).getUniform("u_scale");
    UNI(RECT_COL_COLOR) = SHD(RECT_COL).getUniform("u_color");

}

void close()
{
    glDeleteBuffers(1, &BUF(RECTANGLE_VBO));
    glDeleteVertexArrays(1, &VAO(RECTANGLE));
}

}

