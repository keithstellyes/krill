#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram(VertexShader& v, FragmentShader& f)
{
    this->programId = glCreateProgram();

    // Attach our shaders to our program
    glAttachShader(this->programId, v.getShaderId());
    glAttachShader(this->programId, f.getShaderId());

    // Link our program
    glLinkProgram(this->programId);
}

void ShaderProgram::use()
{
    glUseProgram(this->programId);
}

int ShaderProgram::getUniformLocation(const char *name)
{
    int uniformLoc = glGetUniformLocation(this->programId, name);
    if(uniformLoc == -1) {
        throw new std::runtime_error("Failed to find uniform!");
    }
    return uniformLoc;
}

void ShaderProgram::set(const char *name, float f)
{
    this->set(this->getUniformLocation(name), f);
}
void ShaderProgram::set(int uniformLoc, float f)
{
    glUniform1f(uniformLoc, f);
}

