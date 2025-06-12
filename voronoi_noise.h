#include <cmath>
#include <cstdlib>
#include <algorithm>

float*** voronoi(float* samplePointsX, float* samplePointsY, float* samplePointsZ, int samplePointsLength, int scale) {

    float*** voronoiMap = (float***)malloc(scale * sizeof(float**));
    for (int i = 0; i < scale; ++i) {
        voronoiMap[i] = (float**)malloc(scale * sizeof(float*));
        for (int j = 0; j < scale; ++j) {
            voronoiMap[i][j] = (float*)malloc(scale * sizeof(float));
        }
    }

    float globalMaxDist = 0.0f;
    for (int i = 0; i < samplePointsLength; i++) {
        float sampleX = samplePointsX[i] * scale;
        float sampleY = samplePointsY[i] * scale;
        float sampleZ = samplePointsZ[i] * scale;

        float dx = std::max(sampleX, (float)scale - sampleX);
        float dy = std::max(sampleY, (float)scale - sampleY);
        float dz = std::max(sampleZ, (float)scale - sampleZ);

        float dist = sqrt(dx * dx + dy * dy + dz * dz);
        if (dist > globalMaxDist) globalMaxDist = dist;
    }

    for (int x = 0; x < scale; x++) {
        for (int y = 0; y < scale; y++) {
            for (int z = 0; z < scale; z++) {

                float minDist = 1e9;

                for (int i = 0; i < samplePointsLength; i++) {
                    float sx = samplePointsX[i] * scale;
                    float sy = samplePointsY[i] * scale;
                    float sz = samplePointsZ[i] * scale;

                    float dx = x - sx, dy = y - sy, dz = z - sz;
                    float dist = sqrt(dx * dx + dy * dy + dz * dz);
                    if (dist < minDist) minDist = dist;
                }

                voronoiMap[x][y][z] = minDist / globalMaxDist;
            }
        }
    }

    return voronoiMap;
}