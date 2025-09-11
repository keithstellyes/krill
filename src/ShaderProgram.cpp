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
    // ensure this program is currently being used
    // this way we maintain the abstraction
    this->use();
    if(this->uniformLocCache.count(name)) {
        return uniformLocCache[name];
    }
    int uniformLoc = glGetUniformLocation(this->programId, name);
    if(uniformLoc == -1) {
        char msgbuff[50];
        snprintf(msgbuff, 50, "Failed to find uniform: %s", name);
        throw new std::runtime_error(msgbuff);
    }
    uniformLocCache[name] = uniformLoc;
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

void ShaderProgram::set(const char *name, int i)
{
    set(this->getUniformLocation(name), i);
}

void ShaderProgram::set(int uniformLoc, int i)
{
    glUniform1i(uniformLoc, i);
}
