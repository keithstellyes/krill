#include "Vertex3d.hpp"
namespace krill {
    std::ostream& operator<<(std::ostream &os, const Vertex3d &v)
    {
        os << '(' << v.x << ',' << v.y << ',' << v.z << ')';
        return os;
    }
}
