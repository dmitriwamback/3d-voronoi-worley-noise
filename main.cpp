#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "voronoi_noise.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <zlib.h>
#include <cstdint>
#include <arpa/inet.h>

// write to an image
void write_chunk(std::ofstream& out, const char* type, const std::vector<uint8_t>& data) {
    uint32_t len = htonl(data.size());
    out.write((char*)&len, 4);
    out.write(type, 4);
    out.write((char*)data.data(), data.size());

    uint32_t crc = crc32(0, (const Bytef*)type, 4);
    crc = crc32(crc, data.data(), data.size());
    uint32_t crc_be = htonl(crc);
    out.write((char*)&crc_be, 4);
}

void write_grayscale_png(const char* filename, const std::vector<uint8_t>& pixels, int width, int height) {
    std::ofstream out(filename, std::ios::binary);
    out.write("\x89PNG\r\n\x1A\n", 8);

    // IHDR
    std::vector<uint8_t> ihdr(13);
    uint32_t w = htonl(width), h = htonl(height);
    memcpy(&ihdr[0], &w, 4);
    memcpy(&ihdr[4], &h, 4);
    ihdr[8] = 8;   // bit depth
    ihdr[9] = 0;   // grayscale
    ihdr[10] = 0;  // compression
    ihdr[11] = 0;  // filter
    ihdr[12] = 0;  // no interlace
    write_chunk(out, "IHDR", ihdr);

    std::vector<uint8_t> raw((width + 1) * height);
    for (int y = 0; y < height; ++y) {
        raw[y * (width + 1)] = 0;
        memcpy(&raw[y * (width + 1) + 1], &pixels[y * width], width);
    }

    uLongf compLen = compressBound(raw.size());
    std::vector<uint8_t> compData(compLen);
    compress2(compData.data(), &compLen, raw.data(), raw.size(), Z_BEST_COMPRESSION);
    compData.resize(compLen);

    write_chunk(out, "IDAT", compData);
    write_chunk(out, "IEND", {});
    out.close();
}




int main() {

    int sampleSize = 100;
    float xsample[sampleSize], ysample[sampleSize], zsample[sampleSize];

    int size = 100;

    for (int i = 0; i < sampleSize; i++) {
        float x = (float)rand() / RAND_MAX,
              y = (float)rand() / RAND_MAX,
              z = (float)rand() / RAND_MAX;

        xsample[i] = x;
        ysample[i] = y;
        zsample[i] = z;
    }

    float ***voronoiMap = voronoi(xsample, ysample, zsample, sampleSize, size);

    std::vector<uint8_t> image(size * size);
    int z = size / 2;
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            float v = voronoiMap[x][y][z];
            image[y * size + x] = static_cast<uint8_t>(v * 255);
        }
    }

    write_grayscale_png("voronoi_slice.png", image, size, size);

    for (int x = 0; x < size; ++x) for (int y = 0; y < size; ++y) delete[] voronoiMap[x][y];
    for (int x = 0; x < size; ++x) delete[] voronoiMap[x];
    delete[] voronoiMap;

}