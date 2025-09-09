#include "Buffer.hpp"

Buffer::Buffer(GLenum target)
{
    glGenBuffers(1, &this->bufferId);
    this->target = target;
}

size_t Buffer::bytelen() const
{
    return this->memberSize * this->numMembers;
}

void Buffer::bind()
{
    glBindBuffer(this->target, this->bufferId);
}

// TODO
// This assumes we've set our buffers to be laid out where it's
// {vertex1.attribute1, v1.a2, v1.a3, ... v1.aN, v2.1, ... vN.N}
// Fine if it's a static draw and not regularly mutated
// But if one attribute is regularly mutated but not others then this is not ideal if we want to use glBufferSubData
// Instead, it would potentially be better to use:
// {vertex1.attribute1, v2.a1, v3.a1, ... vN.a1, ..., vN.N}
// In other words, storing attributes together contigiously versus vertexes
void ArrayBuffer::useVertexDefinition(const VertexDefinition& vertexDef)
{
    size_t requiredData = 0;
    size_t stride = vertexDef.getStride();
    size_t offset = 0;
    for(int i = 0; i < vertexDef.getTypes().size(); i++) {
        GLSLType t = vertexDef.getTypes()[i];
        switch(t) {
            case GLSLType::Float:
                glVertexAttribPointer(i, 1, GL_FLOAT, GL_FALSE, stride, (const void*)offset);
                break;
            case GLSLType::Vec2:
                glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, stride, (const void*)offset);
                break;
            case GLSLType::Vec3:
                glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, stride, (const void*)offset);
                break;
        }
        glEnableVertexAttribArray(i);
        offset += getGLSLTypeSize(t);
    }
}
