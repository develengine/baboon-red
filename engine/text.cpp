#include "text.hpp"

#include "shaders.hpp"
#include "textures.hpp"
#include "oglcore.hpp"

#include <glad/glad.h>

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
    glGenVertexArrays(1, &ENG_VAO(TEXT));
    glBindVertexArray(ENG_VAO(TEXT));

    glGenBuffers(1, &ENG_BUF(TEXT_VBO));
    glBindBuffer(GL_ARRAY_BUFFER, ENG_BUF(TEXT_VBO));
    glBufferData(GL_ARRAY_BUFFER, sizeof(textRectangleVertices), textRectangleVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ENG_BUF(TEXT_STRING));
    glBindBuffer(GL_ARRAY_BUFFER, ENG_BUF(TEXT_STRING));
    glBufferData(GL_ARRAY_BUFFER, TEXT_MEMORY_SIZE, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(u8), (void*)0);
    glEnableVertexAttribArray(2);

    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // Texture
    int width, height, channelCount;
    stbi_set_flip_vertically_on_load(false);
    u8 *img = stbi_load("../cringe/charsheet.png", &width, &height, &channelCount, STBI_rgb_alpha);
    if (!img) {
        std::cerr << "Failed to load image\n";
        exit(-1);
    }

    Texture::Parameters parameters
    {
        img,
        width, height,
        GL_REPEAT, GL_REPEAT,
        GL_NEAREST, GL_NEAREST,
        GL_RGBA, GL_RED, GL_UNSIGNED_BYTE,
        false
    };

    Texture::generate(ENG_TEX(CHARSHEET), parameters);

    free(img);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ENG_TEX(CHARSHEET));

    // Shader
    Shader::generate(
        ENG_PRG(TEXT),
        ENG_SHD(TEXT_VERTEX), ENG_SHD(TEXT_FRAGMENT),
        Shader::load("shaders/text.vert"),
        Shader::load("shaders/text.frag")
    );

    ENG_UNI(TEXT_TRANSFORM)  = glGetUniformLocation(ENG_PRG(TEXT), "u_transform");
    ENG_UNI(TEXT_COLOR)      = glGetUniformLocation(ENG_PRG(TEXT), "u_textColor");
    ENG_UNI(TEXT_BACKGROUND) = glGetUniformLocation(ENG_PRG(TEXT), "u_backColor");
    ENG_UNI(TEXT_WRAP)       = glGetUniformLocation(ENG_PRG(TEXT), "u_wrapLen");

    glUseProgram(ENG_PRG(TEXT));
    glUniform1i(glGetUniformLocation(ENG_PRG(TEXT), "u_charSheet"), 1);
    glUseProgram(0);
}

void close()
{
    Shader::destroy(ENG_PRG(TEXT), ENG_SHD(TEXT_VERTEX), ENG_SHD(TEXT_FRAGMENT));
    glDeleteBuffers(1, &ENG_BUF(TEXT_VBO));
    glDeleteBuffers(1, &ENG_BUF(TEXT_STRING));
    glDeleteVertexArrays(1, &ENG_VAO(TEXT));
    glDeleteTextures(1, &ENG_TEX(CHARSHEET));
}

}

