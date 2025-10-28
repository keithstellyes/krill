#include <iostream>
#include <optional>
#include "Stl.hpp"
#include <bit>
#include <locale>
#include "krill.hpp"

using namespace::krill;

int main(int argc, char *argv[])
{
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path to STL file>" << std::endl;
        return 1;
    }

    std::filesystem::path path(argv[1]);
    StlModel model(path);

    std::cout.imbue(std::locale(""));
    if(model.getHeaderType() == StlHeaderType::SolidName) {
        std::cout << "ASCII STL\n";
        std::cout << "Solid name:" << model.getSolidName() << '\n';
    }
    std::cout << model.getTriangleCount() << " triangles\n";
    std::cout << model.getTriangleCount() * 3 << " vertices" << std::endl;
    return 0;
}

