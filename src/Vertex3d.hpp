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
}
