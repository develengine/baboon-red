#include "textures.hpp"

#include <glad/glad.h>

namespace Texture {

void generate(u32 &texture, const Parameters &parameters)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, parameters.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, parameters.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, parameters.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, parameters.magFilter);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        parameters.deviceFormat,
        parameters.width,
        parameters.height,
        0,
        parameters.localFormat,
        parameters.formatType,
        parameters.data
    );

    if (parameters.mipmap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

}
