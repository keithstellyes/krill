#include "VAO.hpp"

VAO::VAO()
{
    glGenVertexArrays(1, &this->id);
    this->bind();
}

void VAO::bind()
{
    glBindVertexArray(this->id);
}
