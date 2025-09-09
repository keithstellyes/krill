#pragma once

#include "Shader.hpp"

class ShaderProgram {
    private:
        GLuint programId;
    public:
        ShaderProgram(VertexShader &vertexShader, FragmentShader &fragmentShader);
        GLuint getProgramId() { return this->programId; }
        void use();
        int getUniformLocation(const char *name);
        void set(const char *name, float f);
        void set(int uniformLoc, float f);
};
