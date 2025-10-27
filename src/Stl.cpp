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

/*
 * 0 or more whitespace preceding 'solid', with a 'facet' soon after
 */
static StlHeaderType guessStlType(std::fstream &f)
{
    auto beginning = f.tellg();
    char sample[101];
    f.read(sample, 100);
    f.seekg(beginning);
    sample[100] = '\0';
    std::string str(sample);
    // skip initial whitespace
    while(!str.empty() && (str[0] == ' ' || str[0] == '\n' || str[0] == '\r' || str[0] == '\n')) {
        str.erase(0, 1);
    }
    if(str.find("solid") != 0) {
        return StlHeaderType::RawData;
    }
    // consume token
    str.erase(0, strlen("solid "));
    if(str.find("facet") == std::string::npos) {
        return StlHeaderType::RawData;
    } else {
        return StlHeaderType::SolidName;
    }
}

void StlModel::parse(std::fstream &f)
{
    StlHeaderType t = guessStlType(f);
    if(t == StlHeaderType::SolidName) {
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

