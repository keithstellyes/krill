#include "VertexDefinition.hpp"
#include <cassert>

void VertexDefinition::add(GLSLType datatype)
{
    this->types.push_back(datatype);
}

void VertexDefinition::addFloat()
{
    this->types.push_back(GLSLType::Float);
}
void VertexDefinition::addVec2()
{
    this->types.push_back(GLSLType::Vec2);
}
void VertexDefinition::addVec3()
{
    this->types.push_back(GLSLType::Vec3);
}

const std::vector<GLSLType>& VertexDefinition::getTypes() const
{
    return this->types;
}

size_t VertexDefinition::getStride() const
{
    size_t total = 0;
    for(const GLSLType t : this->types) {
        total += getGLSLTypeSize(t);
    }

    return total;
}

size_t getGLSLTypeSize(GLSLType datatype)
{
    switch(datatype) {
        case GLSLType::Float:
            return 4;
        case GLSLType::Vec2:
            return getGLSLTypeSize(GLSLType::Float) * 2;
        case GLSLType::Vec3:
            return getGLSLTypeSize(GLSLType::Float) * 3;
        default:
            assert(false);
    }
}
