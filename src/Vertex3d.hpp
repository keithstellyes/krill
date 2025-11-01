#pragma once

#include <ostream>
namespace krill {
    class Vertex3d {
        public:
            double x, y, z;
            constexpr Vertex3d() : x(0), y(0), z(0) {}
            constexpr Vertex3d(double x, double y, double z) : x(x), y(y), z(z) {}
    };

    std::ostream& operator<<(std::ostream &os, const Vertex3d &v);
    constexpr Vertex3d operator*(const Vertex3d &lhs, double rhs)
    {
        Vertex3d result = lhs;
        result.x *= rhs;
        result.y *= rhs;
        result.z *= rhs;
        return result;
    }

    constexpr Vertex3d& operator*=(Vertex3d &lhs, double rhs)
    {
        lhs.x *= rhs;
        lhs.y *= rhs;
        lhs.z *= rhs;

        return lhs;
    }

    constexpr Vertex3d& operator-=(Vertex3d &lhs, const Vertex3d &rhs)
    {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        lhs.z -= rhs.z;
        return lhs;
    }

}
