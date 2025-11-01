#pragma once

#include <cmath>
#include <ostream>
#include <vector>
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

    class BoundingBox {
        public:
            Vertex3d minVert = Vertex3d(INFINITY, INFINITY, INFINITY);
            Vertex3d maxVert = Vertex3d(-INFINITY, -INFINITY, -INFINITY);
            constexpr BoundingBox() {}
            constexpr BoundingBox(const std::vector<Triangle> &triangles)
            {
                for(const Triangle &t : triangles) {
                    for(const Vertex3d &v : t.vertices) {
                        minVert.x = std::min(minVert.x, v.x);
                        maxVert.x = std::max(maxVert.x, v.x);
                        minVert.y = std::min(minVert.y, v.y);
                        maxVert.y = std::max(maxVert.y, v.y);
                        minVert.z = std::min(minVert.z, v.z);
                        maxVert.z = std::max(maxVert.z, v.z);
                    }
                }
            }
            constexpr double maxSize() const
            {
                double xSize = std::abs(maxVert.x - minVert.x);
                double ySize = std::abs(maxVert.y - minVert.y);
                double zSize = std::abs(maxVert.z - minVert.z);
                return std::max(std::max(xSize, ySize), zSize);
            }
            constexpr Vertex3d center() const
            {
                double xCenter = (maxVert.x + minVert.x) / 2.0d;
                double yCenter = (maxVert.y + minVert.y) / 2.0d;
                double zCenter = (maxVert.z + minVert.z) / 2.0d;
                return Vertex3d(xCenter, yCenter, zCenter);
            }
    };
    constexpr Triangle operator*(const Triangle &lhs, double rhs)
    {
        Triangle result = lhs;
        result.vertices[0] *= rhs;
        result.vertices[1] *= rhs;
        result.vertices[2] *= rhs;

        return result;
    }
    constexpr Triangle& operator*=(Triangle &lhs, double rhs)
    {
        lhs.vertices[0] *= rhs;
        lhs.vertices[1] *= rhs;
        lhs.vertices[2] *= rhs;

        return lhs;
    }
    constexpr Triangle& operator-=(Triangle &lhs, const Vertex3d &rhs)
    {
        lhs.vertices[0] -= rhs;
        lhs.vertices[1] -= rhs;
        lhs.vertices[2] -= rhs;
        return lhs;
    }

    // scales triangles in-place such that they fit within Vertex3d min and max
    constexpr void normalizeTriangles(std::vector<Triangle> &triangles, const BoundingBox &constrainingBox)
    {
        // step 1: calculate bounding box
        BoundingBox bb(triangles);
        // step 2 get scale factor of original model
        // TODO: I think this assumes we want scale of size 0,1 maybe it should be
        // constrainingBox.maxSize() / bb.maxSize() ?
        double scaleFactor = 1.0d / bb.maxSize();
        for(unsigned int i = 0; i < triangles.size(); i++) {
            triangles[i] *= scaleFactor;
        }
        // in theory it would be more efficient to calculate the center without this call
        // to BoundingBox, but this is "fast enough"
        bb = BoundingBox(triangles);
        for(unsigned int i = 0; i < triangles.size(); i++) {
            triangles[i] -= bb.center();
        }
    }
    std::ostream& operator<<(std::ostream &os, const Triangle &t);
}
