#pragma once

#include "Shader.hpp"
#include <map>

class ShaderProgram {
    private:
        GLuint programId;
        std::map<const char*, int> uniformLocCache;
    public:
        ShaderProgram(VertexShader &vertexShader, FragmentShader &fragmentShader);
        GLuint getProgramId() { return this->programId; }
        void use();
        int getUniformLocation(const char *name);
        void set(const char *name, float f);
        void set(const char *name, int i);
        void set(int uniformLoc, int i);
        void set(int uniformLoc, float f);
};
