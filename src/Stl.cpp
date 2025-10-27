#include "Stl.hpp"
#include <bit>

using namespace::krill;

StlModel::StlModel(std::filesystem::path path)
{
    this->parse(path);
}

void StlModel::parse(std::filesystem::path path)
{
    std::fstream f(path, std::ios::in);
    this->parse(f);
}

void StlModel::parse(std::fstream &f)
{
    auto beginning = f.tellg();
    const char *magicNumber = "solid ";
    // magic number does NOT include a null terminator,
    size_t magicNumberLen = strlen(magicNumber);

    char buf[magicNumberLen];
    f.read(buf, magicNumberLen);

    f.seekg(beginning);
    if(memcmp(buf, magicNumber, magicNumberLen) == 0) {
        this->parseAscii(f);
    } else {
        this->parseBinary(f);
    }
}

/*
 * Taken from https://stackoverflow.com/questions/2782725/converting-float-values-from-big-endian-to-little-endian/2782742#2782742
 */
constexpr float fromLittle( const float inFloat )
{
    if(std::endian::native == std::endian::big) {
        float retVal;
        char *floatToConvert = ( char* ) & inFloat;
        char *returnFloat = ( char* ) & retVal;

        // swap the bytes into a temporary buffer
        returnFloat[0] = floatToConvert[3];
        returnFloat[1] = floatToConvert[2];
        returnFloat[2] = floatToConvert[1];
        returnFloat[3] = floatToConvert[0];

        return retVal;
    } else {
        return inFloat;
    }
}

constexpr uint32_t fromLittle(uint32_t v)
{
    if(std::endian::native == std::endian::big) {
        v = std::byteswap(v);
    }

    return v;
}

void StlModel::parseBinary(std::fstream &f)
{
    StlBinaryHeader header;

    f.read(reinterpret_cast<char*>(&header), sizeof(header));
    header.triangleCount = fromLittle(header.triangleCount);
    this->rawHeader = std::make_unique<uint8_t[]>(80);
    this->rawHeaderLength = 80;
    this->headerType = StlHeaderType::RawData;
    for(unsigned int i = 0; i < header.triangleCount; i++) {
        StlTri tri;
        f.read(reinterpret_cast<char*>(&tri), sizeof(tri));
        if(std::endian::native == std::endian::big) {
            throw std::runtime_error("Swapping tris for native endian not impl");
        }
        this->triangles.push_back(tri);
    }
}
void StlModel::parseAscii(std::fstream &f)
{
    throw std::runtime_error("ASCII parsing not impl");
}

size_t StlModel::getTriangleCount() const
{
    return this->triangles.size();
}

