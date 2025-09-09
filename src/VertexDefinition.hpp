#pragma once

#include <vector>

enum GLSLType {
    Float,
    Vec2,
    Vec3
};

size_t getGLSLTypeSize(GLSLType datatype);

class VertexDefinition {
    private:
        std::vector<GLSLType> types;
    public:
        void add(GLSLType datatype);
        void addFloat();
        void addVec2();
        void addVec3();
        const std::vector<GLSLType>& getTypes() const;
        size_t getStride() const;
};
