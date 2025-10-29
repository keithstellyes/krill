#include "Triangle.hpp"

namespace krill {
    std::ostream& operator<<(std::ostream &os, const Triangle &t)
    {
        os << '[' << t.vertices[0] << ',' << t.vertices[1] << ',' << t.vertices[2] << ']';
        return os;
    }
}
