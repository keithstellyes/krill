#pragma once
#include "Buffer.hpp"

class VAO {
    protected:
        GLuint id;
    public:
        VAO();
        void bind();
};
