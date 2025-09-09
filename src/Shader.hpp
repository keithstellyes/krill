#pragma once

#include <GL/glew.h>

#include <filesystem>
#include <fstream>
#include <string>

class Shader {
    protected:
        GLuint shaderId;
        Shader() {}
    public:
        ~Shader();
        GLuint getShaderId() { return this->shaderId; }
};

class FragmentShader : public Shader {
    public:
        FragmentShader(const char *src);
        FragmentShader(std::ifstream& file);
        FragmentShader(std::filesystem::path srcPath);
};

class VertexShader : public Shader {
    public:
        VertexShader(const char *src);
        VertexShader(std::ifstream& path);
        VertexShader(std::filesystem::path srcPath);
};
