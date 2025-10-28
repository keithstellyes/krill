#include "Stl.hpp"
#include <bit>
#include <regex>
#include <sstream>

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

inline bool consumeToken(std::fstream &f, const char *s)
{
    char buff[strlen(s)];
    f.read(buff, strlen(s));
    buff[strlen(s)] = '\0';
    return strcmp(buff, s) == 0;
}

constexpr StlVertex parseVertex(std::string &s)
{
    StlVertex v;
    size_t nextIndex;
    v.x = std::stof(s, &nextIndex);
    s.erase(0, nextIndex);
    v.y = std::stof(s, &nextIndex);
    s.erase(0, nextIndex);
    v.z = std::stof(s, &nextIndex);
    s.erase(0, nextIndex);
    return v;
}

constexpr std::string parseSolidName(std::string &src)
{
    std::regex solidNameLineRe("^\\s*solid[^\n]*\n");
    std::smatch match;

    // use ^ to ensure it's at start of string, search so entire string doesn't have to match
    if(!std::regex_search(src, match, solidNameLineRe)) {
        throw std::runtime_error("expected ASCII STL to start with solid");
    }
    std::string line = match.str();

    src.erase(0, match.length());
    std::regex solidNamePrefix("^\\s*solid\\s+");
    std::regex_search(line, match, solidNamePrefix);
    line.erase(0, match.length());
    line.erase(line.size() - 1, 1);

    return line;
}

constexpr void consumeRegex(std::string &s, std::smatch &match, const std::regex &r)
{
    if(!std::regex_search(s, match, r)) {
        throw std::runtime_error("Failed to consume expected regex token");
    }
    s.erase(0, match.length());
}

constexpr StlVertex StlModel::parseAsciiLabeledVertex(std::string &s)
{
    std::smatch match;
    try {
        consumeRegex(s, match, std::regex("^\\s*vertex\\s+"));
    } catch(std::runtime_error &e) {
        throw std::runtime_error("Failed to get vertex token");
    }
    return parseVertex(s);
}

constexpr void StlModel::parseAsciiTriangle(std::string &s)
{
    StlTri t;
    std::smatch match;
    if(std::regex_search(s, match, std::regex("^\\s*endsolid.*"))) {
        s.erase(0, s.size());
        return;
    }
    std::regex facetNormalRe("^\\s*facet\\s+normal\\s+");
    try {
        consumeRegex(s, match, facetNormalRe);
    } catch(std::runtime_error &e) {
        std::cerr << s.substr(0, 25) << '\n';
        throw std::runtime_error("Failed to parse facet normal token wehere expected");
    }
    t.normalVector = parseVertex(s);
    try {
        std::regex outerRe("^\\s*outer\\s+loop\\s+");
        consumeRegex(s, match, outerRe);
    } catch(std::runtime_error &e) {
        throw std::runtime_error("Failed to parse outer loop token where expected");
    }
    t.vertices[0] = parseAsciiLabeledVertex(s);
    t.vertices[1] = parseAsciiLabeledVertex(s);
    t.vertices[2] = parseAsciiLabeledVertex(s);
    std::regex endTri("^\\s+endloop\\s+endfacet\\s*");
    try {
        consumeRegex(s, match, endTri);
    } catch(std::runtime_error &e) {
        throw std::runtime_error("Failed to parse endloop endfacet token where expected");
    }
    this->triangles.push_back(t);
}

void StlModel::parseAsciiTriangles(std::string &s)
{
    while(!s.empty()) {
        this->parseAsciiTriangle(s);
    }
}

void StlModel::parseAscii(std::fstream &f)
{
    this->headerType = StlHeaderType::SolidName;
    std::stringstream srcstream;
    srcstream << f.rdbuf();
    std::string src = srcstream.str();
    std::string name = parseSolidName(src);
    char *s = (char*)malloc(name.size() + 1);
    strcpy(s, name.c_str());
    this->rawHeader = std::unique_ptr<unsigned char[]>((unsigned char*)s);
    this->rawHeaderLength = strlen(s) + 1;
    parseAsciiTriangles(src);
}

size_t StlModel::getTriangleCount() const
{
    return this->triangles.size();
}

StlHeaderType StlModel::getHeaderType() const
{
    return this->headerType;
}

std::string StlModel::getSolidName() const
{
    if(headerType != StlHeaderType::SolidName) {
        throw std::runtime_error("Tried to get the solid name of a binary STL");
    }
    char *raw = (char*)this->rawHeader.get();
    char *s = (char*)malloc(strlen(raw) + 1);
    strcpy(s, (char*)(this->rawHeader).get());
    std::string ret(s);
    free(s);
    return ret;
}

