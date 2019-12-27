#include "text.hpp"

#include "oglcore.hpp"

#include <stb_image.h>

#include <iostream>

namespace Text {

float textRectangleVertices[]
{
     1.0f,-1.0f,  1.0f, 1.0f,
     1.0f, 0.0f,  1.0f, 0.0f,
     0.0f, 0.0f,  0.0f, 0.0f,

     0.0f,-1.0f,  0.0f, 1.0f,
     1.0f,-1.0f,  1.0f, 1.0f,
     0.0f, 0.0f,  0.0f, 0.0f
};

const int TEXT_MEMORY_SIZE = 1028 * 32;


void init()
{
    // Vao
    glGenVertexArrays(1, &VAO(TEXT));
    glBindVertexArray(VAO(TEXT));

    glGenBuffers(1, &BUF(TEXT_VBO));
    glBindBuffer(GL_ARRAY_BUFFER, BUF(TEXT_VBO));
    glBufferData(GL_ARRAY_BUFFER, sizeof(textRectangleVertices), textRectangleVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &BUF(TEXT_STRING));
    glBindBuffer(GL_ARRAY_BUFFER, BUF(TEXT_STRING));
    glBufferData(GL_ARRAY_BUFFER, TEXT_MEMORY_SIZE, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(u8), (void*)0);
    glEnableVertexAttribArray(2);

    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // Texture
    int width, height, channelCount;
    u8 *img = stbi_load("../cringe/charsheet.png", &width, &height, &channelCount, STBI_rgb_alpha);
    if (!img) {
        std::cerr << "Failed to load image\n";
        exit(-1);
    }

    TexParameters parameters = {
        img,
        width, height,
        GL_REPEAT, GL_REPEAT,
        GL_NEAREST, GL_NEAREST,
        GL_RGBA, GL_RED, GL_UNSIGNED_BYTE
    };

    TEX(CHARSHEET).generate(parameters, false);

    free(img);

    TEX(CHARSHEET).bind(1, true);


    // Shader
    SHD(TEXT).generate(
        GraphicShader::load("shaders/text.vert"),
        GraphicShader::load("shaders/text.frag")
    );

    UNI(TEXT_TRANSFORM) = SHD(TEXT).getUniform("u_transform");
    UNI(TEXT_COLOR) = SHD(TEXT).getUniform("u_textColor");
    UNI(TEXT_BACKGROUND) = SHD(TEXT).getUniform("u_backColor");
    UNI(TEXT_WRAP)  = SHD(TEXT).getUniform("u_wrapLen");

    SHD(TEXT).use();
    glUniform1i(SHD(TEXT).getUniform("u_charSheet"), 1);
    GraphicShader::useNone();
}

void close()
{
    glDeleteBuffers(1, &BUF(TEXT_VBO));
    glDeleteBuffers(1, &BUF(TEXT_STRING));
    glDeleteVertexArrays(1, &VAO(TEXT));
}

}

