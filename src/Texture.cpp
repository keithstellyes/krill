#include "Texture.hpp"

Texture::Texture(int width, int height, unsigned char *img)
{
    glGenTextures(1, &this->id);

    glBindTexture(GL_TEXTURE_2D, this->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);
    this->width = width;
    this->height = height;
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, this->id);
}
