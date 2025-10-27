#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <iostream>
#include <vector>
using std::unique_ptr;

namespace krill {

    struct StlVertex {
        float x, y, z;
    } __attribute__((packed));
    struct StlTri {
        StlVertex normalVector;
        StlVertex vertices[3];
        uint16_t attributeByteCount;
    } __attribute__((packed));

    struct StlBinaryHeader {
        uint8_t header[80];
        uint32_t triangleCount;
    } __attribute__((packed));

    static_assert(sizeof(StlVertex) == sizeof(float) * 3);
    static_assert(sizeof(StlTri) == 50);
    static_assert(sizeof(StlBinaryHeader) == 84);

    enum class StlHeaderType {
        SolidName,
        RawData
    };
    class StlModel {
        private:
            unique_ptr<uint8_t[]> rawHeader;
            size_t rawHeaderLength;
            StlHeaderType headerType;
            std::vector<StlTri> triangles;
            void parse(std::filesystem::path path);
            void parse(std::fstream &f);
            void parseBinary(std::fstream &f);
            void parseAscii(std::fstream &f);
        public:
            StlModel(std::filesystem::path);
            size_t getTriangleCount() const;
    };
}
