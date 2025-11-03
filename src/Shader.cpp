#include "Shader.hpp"
#include <iostream>

GLuint compileShader(const char *src, GLenum shaderType)
{
    std::cout << "About to glCreateShader" << std::endl;
    GLuint shaderId = glCreateShader(shaderType);
    std::cout << "Done." << std::endl;
    glShaderSource(shaderId, 1, &src, nullptr);
    glCompileShader(shaderId);
    std::cout << "glCompileShader... Done." << std::endl;

    int  success;
    char infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cerr << "Error occurred while compiling shader: " << infoLog << std::endl;
        throw new std::runtime_error(infoLog);
    }
    return shaderId;
}

GLuint compileShaderFromFile(std::ifstream& file, GLenum shaderType)
{
    file.seekg(0, std::ios::end);
    std::size_t size = file.tellg();
    std::string buffer(size+1, '\0');
    file.seekg(0);
    file.read(&buffer[0], size);

    return compileShader(buffer.c_str(), shaderType);
}

FragmentShader::FragmentShader(const char *src)
{
    this->shaderId = compileShader(src, GL_FRAGMENT_SHADER);
}

VertexShader::VertexShader(const char *src)
{
    this->shaderId = compileShader(src, GL_VERTEX_SHADER);
}

Shader::~Shader()
{
    glDeleteShader(this->shaderId);
}

FragmentShader::FragmentShader(std::ifstream& file)
{
    this->shaderId = compileShaderFromFile(file, GL_FRAGMENT_SHADER);
}

VertexShader::VertexShader(std::ifstream& file)
{
    this->shaderId = compileShaderFromFile(file, GL_VERTEX_SHADER);
}

FragmentShader::FragmentShader(std::filesystem::path srcPath)
{
    std::ifstream f(srcPath);
    this->shaderId = compileShaderFromFile(f, GL_FRAGMENT_SHADER);
    f.close();

}

VertexShader::VertexShader(std::filesystem::path srcPath)
{
    std::ifstream f(srcPath);
    this->shaderId = compileShaderFromFile(f, GL_VERTEX_SHADER);
    f.close();

}

ComputeShader::ComputeShader (const char *src)
{
    this->shaderId = compileShader(src, GL_COMPUTE_SHADER);
}
ComputeShader::ComputeShader(std::ifstream& file)
{
    this->shaderId = compileShaderFromFile(file, GL_COMPUTE_SHADER);
}
ComputeShader::ComputeShader(std::filesystem::path srcPath)
{
    std::ifstream f(srcPath);
    this->shaderId = compileShaderFromFile(f, GL_COMPUTE_SHADER);
    f.close();
}
