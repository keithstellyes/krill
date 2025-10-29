#pragma once

#include <ostream>
#include "Vertex3d.hpp"

namespace krill {
    class Triangle {
        public:
            Vertex3d vertices[3];
            constexpr Triangle() {}
            constexpr Triangle(const Vertex3d *verts)
            {
                this->vertices[0] = verts[0];
                this->vertices[1] = verts[1];
                this->vertices[2] = verts[2];
            }
            constexpr Triangle(const Vertex3d &a, const Vertex3d &b, const Vertex3d &c)
            {
                this->vertices[0] = a;
                this->vertices[1] = b;
                this->vertices[2] = c;
            }
    };

    std::ostream& operator<<(std::ostream &os, const Triangle &t);
}
