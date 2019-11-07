#include "textures.hpp"

#include <glad/glad.h>

Texture::Texture(const TexParameters &parameters) {
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, parameters.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, parameters.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, parameters.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, parameters.magFilter);

    glTexImage2D(GL_TEXTURE_2D, 0, parameters.deviceFormat, parameters.width, parameters.height, 0, parameters.deviceFormat, parameters.formatType, parameters.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(uint32_t slot, bool activate) {
    if (activate) {
        glActiveTexture(GL_TEXTURE0 + slot);
    }
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void Texture::free() {
    if (textureId != 0) {
        glDeleteTextures(1, &textureId);
        textureId = 0;
    }
}

Texture::~Texture() {
    free();
}

