#pragma once
#include <GL/glew.h>

class Texture {
    protected:
        GLuint id;
        unsigned int width, height;
    public:
        Texture(int width, int height, unsigned char *img);
        void bind();
};
