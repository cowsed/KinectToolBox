#ifndef KINECT_TYPES_H
#define KINECT_TYPES_H

#include <cstdint>
#include <vector>

struct vec3 {
    float x, y, z;
};

struct Point {
    uint8_t r, g, b;
    uint16_t depth;
};

using PointCapture = std::vector<Point>;

enum class VideoType {
    RGB,
    IR,
};

#endif // KINECT_TYPES_H
