#pragma once

#include "VertexDefinition.hpp"

#include <GL/glew.h>
#include <vector>

class Buffer {
    protected:
        GLuint bufferId;
        GLenum target;
        size_t memberSize;
        size_t numMembers;
        void genBuffer();
        Buffer(GLenum target);

    public:
        GLuint getBufferId() { return this->bufferId; }
        Buffer() = delete;
        void bind();
        template <typename T>
            void setData(const std::vector<T> &data, GLenum usage)
        {
            this->memberSize = sizeof(T);
            this->numMembers = data.size();
            size_t bytelen = this->memberSize * this->numMembers;
            glBufferData(this->target, bytelen, data.data(), usage);
        }
        size_t bytelen() const;
};

class ArrayBuffer : public Buffer {
    public:
        ArrayBuffer() : Buffer(GL_ARRAY_BUFFER) {}
        void useVertexDefinition(const VertexDefinition& vertexDef);
};

class ElementArrayBuffer : public Buffer {
    public:
        ElementArrayBuffer() : Buffer(GL_ELEMENT_ARRAY_BUFFER) {}
};

class ShaderStorageBuffer : public Buffer {
    public:
        ShaderStorageBuffer() : Buffer(GL_SHADER_STORAGE_BUFFER) {}
        // only shader storage and atomic counters can use glBindBufferBase(...)
        void bindBase(unsigned int index);
};
